#include "HealingStation.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "NecroSyntex/NecroSyntaxComponents/SubComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"

AHealingStation::AHealingStation()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;
    OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
    OverlapBox->SetupAttachment(RootComponent);
    OverlapBox->SetBoxExtent(FVector(200.f, 200.f, 150.f));
    bCanUse = true;
}

void AHealingStation::BeginPlay()
{
    Super::BeginPlay();
    if (OverlapBox)
    {
        OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AHealingStation::OnOverlapBegin);
        OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AHealingStation::OnOverlapEnd);
    }
}

void AHealingStation::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    APlayerCharacter* OverlappingPlayer = Cast<APlayerCharacter>(OtherActor);
    if (OverlappingPlayer)
    {
        if (HasAuthority())
        {
            OverlappingPlayers.Add(OverlappingPlayer);
        }

        OverlappingPlayer->SetHealingStationActor(this);
    }
}


void AHealingStation::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    APlayerCharacter* OverlappingPlayer = Cast<APlayerCharacter>(OtherActor);
    if (OverlappingPlayer)
    {
        if (HasAuthority())
        {
            OverlappingPlayers.Remove(OverlappingPlayer);
        }

        OverlappingPlayer->SetHealingStationActor(nullptr);
    }
}

void AHealingStation::Interact(APlayerCharacter* PlayerCharacter)
{
    if (PlayerCharacter)
    {
        if (GetLocalRole() < ROLE_Authority)
        {
            ServerInteract(PlayerCharacter);
        }
        else
        {
            ServerInteract(PlayerCharacter);
        }
    }
}

bool AHealingStation::ServerInteract_Validate(APlayerCharacter* PlayerCharacter)
{
    return true;
}

void AHealingStation::ServerInteract_Implementation(APlayerCharacter* PlayerCharacter)
{
    if (!bCanUse) return;
    if (!OverlappingPlayers.Contains(PlayerCharacter)) return;
    HealAllPlayers();
    bCanUse = false;
    OnRep_bCanUse();
    GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AHealingStation::ResetCooldown, CooldownTime, false);
}

void AHealingStation::HealAllPlayers()
{
    UWorld* World = GetWorld();
    if (!World) return;
    for (TActorIterator<APlayerCharacter> It(World); It; ++It)
    {
        APlayerCharacter* PC = *It;
        if (PC && !PC->IsElimed())
        {
            float MissingHP = PC->GetMaxHealth() - PC->GetHealth();
            if (MissingHP > 0.f)
            {
                PC->GetSubComp()->Heal(MissingHP, 3.f);
            }
        }
    }
}

void AHealingStation::ResetCooldown()
{
    bCanUse = true;
    OnRep_bCanUse();
}

void AHealingStation::OnRep_bCanUse()
{
}

void AHealingStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AHealingStation, bCanUse);
}
