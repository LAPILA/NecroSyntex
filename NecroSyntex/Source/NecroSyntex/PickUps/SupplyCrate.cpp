#include "SupplyCrate.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/NecroSyntaxComponents/CombatComponent.h"
#include "NecroSyntex/Weapon/Weapon.h"
#include "EngineUtils.h"

#define SC_LOG(Format, ...) UE_LOG(LogTemp, Warning, TEXT("[SupplyCrate] " Format), ##__VA_ARGS__)

ASupplyCrate::ASupplyCrate()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(RootScene);

    CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
    CrateMesh->SetupAttachment(RootScene);
    CrateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CrateMesh->SetRenderCustomDepth(true);
    CrateMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);   // ▶ Outline

    InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
    InteractSphere->SetupAttachment(RootScene);
    InteractSphere->SetSphereRadius(180.f);
    InteractSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    GlowFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowFX"));
    GlowFX->SetupAttachment(RootScene);
}

void ASupplyCrate::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        InteractSphere->OnComponentBeginOverlap.AddDynamic(
            this, &ASupplyCrate::HandleOverlap);
    }
}

/* ------------------------------------------------- *
 * Overlap → PlayerCharacter 에게 “이 상자” 등록
 * ------------------------------------------------- */
void ASupplyCrate::HandleOverlap(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent*, int32, bool,
    const FHitResult&)
{
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
    {
        PC->SetOverlappingSupplyCrate(this);

        if (!GetOwner())
        {
            SetOwner(PC);
        }
    }
}

/* ------------------------------------------------- *
 *  Interact (Client or Server)
 * ------------------------------------------------- */
void ASupplyCrate::Interact(APlayerCharacter* InteractingPC)
{
    if (!InteractingPC || bAlreadyProcessed) return;

    if (GetLocalRole() < ROLE_Authority)
        ServerOpenCrate(InteractingPC);
    else
        ServerOpenCrate(InteractingPC);
}

bool ASupplyCrate::ServerOpenCrate_Validate(APlayerCharacter*) { return true; }

void ASupplyCrate::ServerOpenCrate_Implementation(APlayerCharacter*)
{
    if (bAlreadyProcessed) return;
    bAlreadyProcessed = true;

    SpawnRandomWeapon();
    GiveAmmoToAllPlayers();

    bOpened = true;
    OnRep_Opened();                  // 서버측에서도 FX 재생
}

/* ------------------------------------------------- *
 *  Loot 스폰
 * ------------------------------------------------- */
void ASupplyCrate::SpawnRandomWeapon()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const float Roll = FMath::FRand();

    auto MakeImpulseDir = []() -> FVector
        {
            // 수평 랜덤 + 위쪽 0.7 혼합  → 항상 위쪽으로 살짝 뜸
            FVector Horz = FMath::VRand();  Horz.Z = 0.f;  Horz.Normalize();
            return (Horz + FVector(0, 0, 0.7f)).GetSafeNormal();
        };

    auto SpawnWeapon = [&](TSubclassOf<AWeapon> Class)
        {
            if (!Class) return;

            const FVector ImpulseDir = MakeImpulseDir();
            const float  Lift = CrateMesh->Bounds.BoxExtent.Z + 100.f;
            const FVector SpawnLoc = GetActorLocation() + FVector(0, 0, Lift);

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AWeapon* W = World->SpawnActor<AWeapon>(Class, SpawnLoc, GetActorRotation(), Params);
            if (!W) { SC_LOG("   !! Spawn FAILED"); return; }

            if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(W->GetRootComponent()))
            {
                Root->SetSimulatePhysics(true);
                Root->SetEnableGravity(true);
                Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                Root->WakeAllRigidBodies();
                Root->AddImpulse(ImpulseDir * 900.f, NAME_None, true);
            }
            SC_LOG("   -> %s  Spawn@%s  Impulse=%s",
                *W->GetClass()->GetName(),
                *SpawnLoc.ToCompactString(),
                *ImpulseDir.ToCompactString());
        };

    /* ---- 확률 판정 ---- */
    if (Roll <= NormalWeaponChance && NormalWeaponClasses.Num())
        SpawnWeapon(NormalWeaponClasses[FMath::RandHelper(NormalWeaponClasses.Num())]);

    else if (Roll <= NormalWeaponChance + SpecialWeaponChance && SpecialWeaponClasses.Num())
        SpawnWeapon(SpecialWeaponClasses[FMath::RandHelper(SpecialWeaponClasses.Num())]);
}

 /* ------------------------------------------------- *
  *  모든 플레이어에게 ‘현재 무기 타입’ 1 Mag 지급
  * ------------------------------------------------- */
void ASupplyCrate::GiveAmmoToAllPlayers()
{
    for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
    {
        APlayerCharacter* PC = *It;
        if (!PC) continue;

        /** 플레이어가 장착한 무기와 Combat 컴포넌트 */
        AWeapon* CurWeapon = PC->GetEquippedWeapon();
        UCombatComponent* Combat = PC->GetCombat();
        if (!CurWeapon || !Combat) continue;

        const EWeaponType  Type = CurWeapon->GetWeaponType();
        const int32        Mag = CurWeapon->GetMagCapacity();

        /* ▶ Combat 방식으로 ‘CarriedAmmo’ 증가 */
        Combat->PickUpAmmo(Type, Mag);

        SC_LOG("GiveAmmo: %s  +%d  (Type %d)",
            *PC->GetName(), Mag, static_cast<int32>(Type));
    }
}

/* ------------------------------------------------- *
 *  FX ‑ 사운드, Glow 끄기, 충돌 차단
 * ------------------------------------------------- */
void ASupplyCrate::PlayEffects()
{
    if (OpenSound)
        UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());

    if (GlowFX) GlowFX->Deactivate();
    CrateMesh->SetRenderCustomDepth(false);
    InteractSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASupplyCrate::OnRep_Opened()
{
    if (bOpened)
        PlayEffects();
}

void ASupplyCrate::GetLifetimeReplicatedProps(
    TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASupplyCrate, bOpened);
}