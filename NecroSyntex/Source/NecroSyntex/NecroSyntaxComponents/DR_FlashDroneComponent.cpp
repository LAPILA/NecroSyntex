#include "DR_FlashDroneComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "DR_FlashDrone.h"

UDR_FlashDroneComponent::UDR_FlashDroneComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    static ConstructorHelpers::FClassFinder<ADR_FlashDrone> DroneBP(
        TEXT("/Game/Maincontents/Blueprints/Drone/BP_DR_FlashDrone")
    );
    if (DroneBP.Succeeded())
    {
        FlashDroneClass = DroneBP.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FlashDrone BP not found – check the path"));
    }
}


void UDR_FlashDroneComponent::BeginPlay()
{
    Super::BeginPlay();

    // **서버** 에서만 드론을 생성하고, 다음 틱에 실행
    if (GetOwner()->HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(
            this, &UDR_FlashDroneComponent::SpawnDrone_Internal);
    }
}

void UDR_FlashDroneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (FlashDrone && !FlashDrone->IsPendingKill())
    {
        FlashDrone->Destroy();
    }
    FlashDrone = nullptr;
    Super::EndPlay(EndPlayReason);
}

void UDR_FlashDroneComponent::SpawnDrone_Internal()
{
    if (FlashDrone || !FlashDroneClass) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    FActorSpawnParameters Params;
    Params.Owner = OwnerChar;
    Params.Instigator = OwnerChar;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector SpawnLoc = OwnerChar->GetActorLocation() + FVector(0, 0, 150.f);

    FlashDrone = GetWorld()->SpawnActor<ADR_FlashDrone>(
        FlashDroneClass, SpawnLoc, FRotator::ZeroRotator, Params);

    if (FlashDrone)
    {
        FlashDrone->AttachToActor(OwnerChar, FAttachmentTransformRules::KeepWorldTransform);
        FlashDrone->InitFollowing(OwnerChar, MaxDistanceFromPlayer);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[FlashDroneComp] Drone spawn failed!"));
    }
}

void UDR_FlashDroneComponent::ForceRespawn()
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;

    if (FlashDrone && !FlashDrone->IsPendingKill())
    {
        FlashDrone->Destroy();
    }
    FlashDrone = nullptr;
    SpawnDrone_Internal();
}

void UDR_FlashDroneComponent::GetLifetimeReplicatedProps(
    TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDR_FlashDroneComponent, FlashDrone);
}