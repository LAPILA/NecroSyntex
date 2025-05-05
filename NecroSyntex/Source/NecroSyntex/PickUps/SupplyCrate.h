#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupplyCrate.generated.h"

class USphereComponent;
class UNiagaraComponent;
class USoundCue;
class AWeapon;
class APlayerCharacter;

/**
 *  ────────────────────────────────────────────────
 *  • Glow + Outline 로 가독성
 *  • 열리면 한 번만 처리
 *  • 무기 스폰 확률
 *      - Normal 30 %  (배열 균등)
 *      - Special 20 % (배열 균등)
 *      - 나머지 50 %  : 무기 없음
 *  • 탄약 : 서버에 존재하는 **모든** 플레이어의
 *           ‘현재 장비’ 무기 1 Mag 지급
 *  ────────────────────────────────────────────────
 */
UCLASS()
class NECROSYNTEX_API ASupplyCrate : public AActor
{
    GENERATED_BODY()

public:
    ASupplyCrate();

    /* PlayerCharacter 쪽에서 호출(E 키) */
    UFUNCTION(BlueprintCallable, Category = "Interact")
    void Interact(APlayerCharacter* InteractingPC);

protected:
    virtual void BeginPlay() override;

    /* === 서버 === */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerOpenCrate(APlayerCharacter* InteractingPC);

    /* === Overlap 처리 : 플레이어에게 ‘현재 상자’ 포인터 전달용 === */
    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    /* === 내부 === */
    void SpawnRandomWeapon();   // 무기 스폰
    void GiveAmmoToAllPlayers(); // 탄약 지급
    void PlayEffects();          // 사운드‧FX

    /* === Components === */
    UPROPERTY(VisibleAnywhere) USceneComponent* RootScene;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* CrateMesh;
    UPROPERTY(VisibleAnywhere) USphereComponent* InteractSphere;
    UPROPERTY(VisibleAnywhere) UNiagaraComponent* GlowFX;

    /* === Data === */
    UPROPERTY(EditAnywhere, Category = "Loot") TArray<TSubclassOf<AWeapon>> NormalWeaponClasses;
    UPROPERTY(EditAnywhere, Category = "Loot") TArray<TSubclassOf<AWeapon>> SpecialWeaponClasses;
    UPROPERTY(EditAnywhere, Category = "SFX")  USoundCue* OpenSound;

    /* 확률 (0‑1) */
    UPROPERTY(EditAnywhere, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NormalWeaponChance = 0.30f;
    UPROPERTY(EditAnywhere, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SpecialWeaponChance = 0.20f;

    /* 상태 */
    UPROPERTY(ReplicatedUsing = OnRep_Opened) bool bOpened = false;
    UFUNCTION() void OnRep_Opened();

    bool bAlreadyProcessed = false;   // 안전장치

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
};
