#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupplyCrate.generated.h"

class USphereComponent;
class UNiagaraComponent;
class USoundCue;
class AWeapon;
class APlayerCharacter;

UCLASS()
class NECROSYNTEX_API ASupplyCrate : public AActor
{
    GENERATED_BODY()

public:
    ASupplyCrate();

    UFUNCTION(BlueprintCallable, Category = "Interact")
    void Interact(APlayerCharacter* InteractingPC);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerOpenCrate(APlayerCharacter* InteractingPC);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastOpenCrate();

    void SpawnRandomWeapon();
    void GiveAmmoToAllPlayers();
    void PlayEffects();

    UPROPERTY(VisibleAnywhere) USceneComponent* RootScene;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* CrateMesh;
    UPROPERTY(VisibleAnywhere) USphereComponent* InteractSphere;
    UPROPERTY(VisibleAnywhere) UNiagaraComponent* GlowFX;

    UPROPERTY(EditAnywhere, Category = "Loot") TArray<TSubclassOf<AWeapon>> NormalWeaponClasses;
    UPROPERTY(EditAnywhere, Category = "Loot") TArray<TSubclassOf<AWeapon>> SpecialWeaponClasses;
    UPROPERTY(EditAnywhere, Category = "SFX") USoundCue* OpenSound;

    /* 탄창 개수 설정 */
    UPROPERTY(EditAnywhere, Category = "Ammo") int32 MagCount = 3;

    UPROPERTY(EditAnywhere, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NormalWeaponChance = 0.30f;
    UPROPERTY(EditAnywhere, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SpecialWeaponChance = 0.20f;

    UPROPERTY(ReplicatedUsing = OnRep_Opened) bool bOpened = false;
    UFUNCTION() void OnRep_Opened();

    bool bAlreadyProcessed = false;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
