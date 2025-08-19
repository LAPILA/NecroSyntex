#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DR_FlashDroneComponent.generated.h"

class ADR_FlashDrone;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NECROSYNTEX_API UDR_FlashDroneComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UDR_FlashDroneComponent();

    UFUNCTION(BlueprintPure, Category = "Drone")
    ADR_FlashDrone* GetFlashDrone() const { return FlashDrone; }

    /** ������ ������ ���� �����ϰ� ���� �� ȣ�� */
    UFUNCTION(BlueprintCallable, Category = "FlashDrone")
    void ForceRespawn();


protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& Out) const override;

    /** ���� ���� ���� ���� �Լ� */
    void SpawnDrone_Internal();

    UPROPERTY(Replicated)
    ADR_FlashDrone* FlashDrone = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Drone")
    TSubclassOf<ADR_FlashDrone> FlashDroneClass;

    UPROPERTY(EditDefaultsOnly, Category = "FlashDrone")
    float MaxDistanceFromPlayer = 700.f;
};
