// Copyright NecroSyntex. All Rights Reserved.

#include "DR_FlashDroneComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "DR_FlashDrone.h"

UDR_FlashDroneComponent::UDR_FlashDroneComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UDR_FlashDroneComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(
            this, &UDR_FlashDroneComponent::SpawnDrone_Internal);
    }
}

void UDR_FlashDroneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(FlashDrone))
    {
        FlashDrone->Destroy();
    }
    FlashDrone = nullptr;
    Super::EndPlay(EndPlayReason);
}

void UDR_FlashDroneComponent::SpawnDrone_Internal()
{
    if (!GetOwner() || !GetWorld()) return;
    if (IsValid(FlashDrone) || !FlashDroneClass) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    FActorSpawnParameters Params;
    Params.Owner = OwnerChar;
    Params.Instigator = OwnerChar;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector SpawnLoc = OwnerChar->GetActorLocation() + FVector(0, 0, 150.f);

    FlashDrone = GetWorld()->SpawnActor<ADR_FlashDrone>(
        FlashDroneClass, SpawnLoc, FRotator::ZeroRotator, Params);

    if (IsValid(FlashDrone))
    {
        FlashDrone->InitFollowing(OwnerChar, MaxDistanceFromPlayer);
        FlashDrone->AddTickPrerequisiteActor(OwnerChar);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[FlashDroneComp] Drone spawn failed!"));
    }
}

void UDR_FlashDroneComponent::ForceRespawn()
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;

    if (IsValid(FlashDrone))
    {
        FlashDrone->Destroy();
    }
    FlashDrone = nullptr;
    SpawnDrone_Internal();
}

void UDR_FlashDroneComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDR_FlashDroneComponent, FlashDrone);
}