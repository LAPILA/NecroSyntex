#include "SupplyCrate.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/Weapon/Weapon.h"
#include "NecroSyntex/NecroSyntaxComponents/CombatComponent.h"
#include "NecroSyntex/DopingSystem/DopingComponent.h"
#include "EngineUtils.h"

ASupplyCrate::ASupplyCrate()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(RootScene);

    CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
    CrateMesh->SetupAttachment(RootScene);

    InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
    InteractSphere->SetupAttachment(RootScene);
    InteractSphere->SetSphereRadius(180.f);

    GlowFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowFX"));
    GlowFX->SetupAttachment(RootScene);
}

void ASupplyCrate::BeginPlay()
{
    Super::BeginPlay();
    InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ASupplyCrate::HandleOverlap);
}

void ASupplyCrate::HandleOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
    {
        PC->SetOverlappingSupplyCrate(this);
    }
}

void ASupplyCrate::Interact(APlayerCharacter* InteractingPC)
{
    if (bAlreadyProcessed) return;
    if (HasAuthority()) ServerOpenCrate(InteractingPC);
}

bool ASupplyCrate::ServerOpenCrate_Validate(APlayerCharacter* InteractingPC) { return InteractingPC != nullptr; }

void ASupplyCrate::ServerOpenCrate_Implementation(APlayerCharacter* InteractingPC)
{
    if (bAlreadyProcessed) return;
    bAlreadyProcessed = true;
    GlowFX->Activate();
    SpawnRandomWeapon();
    GiveAmmoToAllPlayers();
    MulticastOpenCrate();
}

void ASupplyCrate::MulticastOpenCrate_Implementation()
{
    bOpened = true;
    PlayEffects();
}

void ASupplyCrate::SpawnRandomWeapon()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const float Roll = FMath::FRand();
    auto SpawnWeapon = [&](TSubclassOf<AWeapon> Class)
        {
            FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100.f);
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // 무기 스폰
            AWeapon* W = World->SpawnActor<AWeapon>(Class, SpawnLoc, FRotator::ZeroRotator, Params);
            if (!W)
            {
                UE_LOG(LogTemp, Warning, TEXT("Spawn FAILED"));
                return;
            }

            // 물리 및 중력 설정 (루트 컴포넌트가 UPrimitiveComponent인지 확인 후 처리)
            if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(W->GetRootComponent()))
            {
                Root->SetSimulatePhysics(true);
                Root->SetEnableGravity(true);
                Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                Root->WakeAllRigidBodies();
                Root->AddImpulse(FVector(0, 0, 300.f), NAME_None, true);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Root is not a UPrimitiveComponent"));
            }
        };

    /* ---- 확률 판정 ---- */
    if (Roll <= NormalWeaponChance && NormalWeaponClasses.Num())
        SpawnWeapon(NormalWeaponClasses[FMath::RandHelper(NormalWeaponClasses.Num())]);
    else if (Roll <= NormalWeaponChance + SpecialWeaponChance && SpecialWeaponClasses.Num())
        SpawnWeapon(SpecialWeaponClasses[FMath::RandHelper(SpecialWeaponClasses.Num())]);
}

void ASupplyCrate::GiveAmmoToAllPlayers()
{
    for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
    {
        APlayerCharacter* PC = *It;
        if (!PC || !PC->GetCombat()) continue;

        TArray<EWeaponType> OwnedWeaponTypes;
        PC->GetCombat()->GetOwnedWeaponTypes(OwnedWeaponTypes);

        for (EWeaponType WeaponType : OwnedWeaponTypes)
        {
            int32 MaxAmmo = PC->GetCombat()->GetMaxAmmoForWeaponType(WeaponType);
            int32 AmmoToGive = MaxAmmo * MagCount;
            PC->GetCombat()->PickUpAmmo(WeaponType, AmmoToGive);
        }

        PC->UDC->One_DopingItemNum += 2;
        PC->UDC->Two_DopingItemNum += 2;
    }
}

void ASupplyCrate::PlayEffects()
{
    if (OpenSound)
        UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
}

void ASupplyCrate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASupplyCrate, bOpened);
}

void ASupplyCrate::OnRep_Opened()
{
    // 플레이 효과 실행
    PlayEffects();
    UE_LOG(LogTemp, Log, TEXT("Supply Crate opened - OnRep_Opened called"));
}