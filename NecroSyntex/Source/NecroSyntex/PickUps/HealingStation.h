#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealingStation.generated.h"

UCLASS()
class NECROSYNTEX_API AHealingStation : public AActor
{
    GENERATED_BODY()

public:
    AHealingStation();

    UFUNCTION(BlueprintCallable)
    void Interact(class APlayerCharacter* PlayerCharacter);

    //duream code add healingmesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* HealingMesh;
protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerInteract(class APlayerCharacter* PlayerCharacter);

    void HealAllPlayers();

    UFUNCTION()
    void ResetCooldown();

    UFUNCTION()
    void OnRep_bCanUse();

    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* OverlapBox;

    UPROPERTY(ReplicatedUsing = OnRep_bCanUse)
    bool bCanUse;

    UPROPERTY(EditAnywhere)
    float CooldownTime = 180.f;

    TSet<class APlayerCharacter*> OverlappingPlayers;
    FTimerHandle CooldownTimerHandle;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
