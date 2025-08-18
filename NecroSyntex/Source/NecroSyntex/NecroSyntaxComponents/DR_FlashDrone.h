// Copyright NecroSyntex. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "DR_FlashDrone.generated.h"

class USpotLightComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class NECROSYNTEX_API ADR_FlashDrone : public AActor
{
    GENERATED_BODY()

public:
    ADR_FlashDrone();
    virtual void Tick(float DeltaTime) override;
    void InitFollowing(AActor* InTarget, float InMaxDist);
    void SetAimTarget(const FVector& NewTarget);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /* ───────── Components ───────── */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> DroneMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpotLightComponent> SpotLight;

    /* ───────── Movement Parameters ───────── */
    UPROPERTY(EditDefaultsOnly, Category = "Drone|Movement", meta = (ToolTip = "드론이 목표를 따라가는 속도입니다. 값을 높이면 더 빠릅니다."))
    float FollowInterpSpeed = 15.f;

    UPROPERTY(EditDefaultsOnly, Category = "Drone|Movement")
    float CheckInterval = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Drone|Orbit") float OrbitRadius = 60.f;
    UPROPERTY(EditAnywhere, Category = "Drone|Orbit") float OrbitHeight = 90.f;
    UPROPERTY(EditAnywhere, Category = "Drone|Orbit") float OrbitSpeed = 60.f;
    UPROPERTY(EditAnywhere, Category = "Drone|Orbit") float PivotRightOffset = 35.f;
    UPROPERTY(EditAnywhere, Category = "Drone|Aiming") FVector AimOffset = FVector(35.f, 55.f, 40.f);

    /* ───────── Replicated State ───────── */
    UPROPERTY(Replicated)
    FVector_NetQuantize CurrentAimTarget;

    UPROPERTY(ReplicatedUsing = OnRep_ServerState)
    FVector_NetQuantize ReplicatedLocation;

    UPROPERTY(ReplicatedUsing = OnRep_ServerState)
    FRotator ReplicatedRotation;

    UPROPERTY(Replicated)
    TObjectPtr<AActor> TargetActor = nullptr;

private:
    /* ───────── RPCs (Remote Procedure Calls) ───────── */
    UFUNCTION(Server, Unreliable)
    void ServerSetAimTarget(const FVector_NetQuantize& NewTarget);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ForceTeleport(const FVector& NewLocation, const FRotator& NewRotation);

    /* ───────── OnRep Notifications ───────── */
    UFUNCTION()
    void OnRep_ServerState();

    /* ───────── Internal Logic ───────── */
    void ForceTeleportToTarget();
    void CheckDistanceAndTeleport();

    FTimerHandle TeleportCheckTimer;
    float MaxDistance = 700.f;
    float OrbitYaw = 0.f;

    // 클라이언트의 부드러운 이동(보간)을 위한 변수들
    FVector InterpolationTargetLocation;
    FRotator InterpolationTargetRotation;
};