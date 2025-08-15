// DR_FlashDrone.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "DR_FlashDrone.generated.h"

class USpotLightComponent;

UCLASS()
class NECROSYNTEX_API ADR_FlashDrone : public AActor
{
    GENERATED_BODY()

public:
    ADR_FlashDrone();

    virtual void Tick(float DeltaTime) override;

    /** 서버‑측 추적 시작 */
    void InitFollowing(AActor* InTarget, float InMaxDist);

    /** 주변이 어두울 때만 가시화 */
    void SetLightActive(bool bActive);

    /** 강제 텔레포트 (플레이어 리스폰 등) */
    UFUNCTION(BlueprintCallable) void ForceTeleportToTarget();

    /** 로컬 → 서버 : 조준 점 갱신 */
    void SetAimTarget(const FVector& NewTarget);
    UFUNCTION(Server, Unreliable) void ServerSetAimTarget(const FVector_NetQuantize& NewTarget);

    UFUNCTION(BlueprintCallable)
    void ToggleFlash(bool bOn);


    UFUNCTION(Server, Reliable)
    void ServerToggleFlash(bool bOn);

protected:
    virtual void BeginPlay() override;

    /* ───────── Components ───────── */
    UPROPERTY(VisibleAnywhere) USceneComponent* RootComp;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* DroneMesh;
    UPROPERTY(VisibleAnywhere) USpotLightComponent* SpotLight;

    /* ───────── Movement params ──────── */
    UPROPERTY(EditDefaultsOnly) float FollowInterpSpeed = 8.f;
    UPROPERTY(EditDefaultsOnly) float CheckInterval = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Drone") float OrbitRadius = 60.f;   // 좌·우 반경
    UPROPERTY(EditAnywhere, Category = "Drone") float OrbitHeight = 90.f;   // 머리 위 높이
    UPROPERTY(EditAnywhere, Category = "Drone") float OrbitSpeed = 60.f;   // deg/sec
    UPROPERTY(EditAnywhere, Category = "Drone|Orbit")
    float PivotRightOffset = 35.f;

    /** 조준 시 오른쪽 어깨 위치 */
    UPROPERTY(EditAnywhere, Category = "Drone")
    FVector AimOffset = FVector(35.f, 55.f, 40.f);   // (앞, 오른쪽, 위)

    /* ───────── Replicated state ─────── */
    /** 클라이언트가 보내는 HitTarget → 서버 복제 */
    UPROPERTY(ReplicatedUsing = OnRep_AimTarget)
    FVector_NetQuantize CurrentAimTarget;

    UFUNCTION() void OnRep_AimTarget();

    /* ───────── Internals ───────── */
    void CheckDistanceAndTeleport();
    void UpdateAutoLight();
    void UpdateLightDirection();

    UPROPERTY()  AActor* TargetActor = nullptr;
    UPROPERTY()  float   MaxDistance = 700.f;

    float OrbitYaw = 0.f;
    bool  bIsAiming = false;
    FTimerHandle TeleportCheckTimer;

    UPROPERTY(ReplicatedUsing = OnRep_FlashOn)
    bool bFlashOn = true;

    UFUNCTION()
    void OnRep_FlashOn();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
