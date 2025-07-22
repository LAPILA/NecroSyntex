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

    ADR_FlashDrone* GetFlashDrone() const { return FlashDrone; }

    /** 언제라도 강제로 새로 스폰하고 싶을 때 호출 */
    UFUNCTION(BlueprintCallable, Category = "FlashDrone")
    void ForceRespawn();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& Out) const override;

    /** 서버 전용 내부 스폰 함수 */
    void SpawnDrone_Internal();

    UPROPERTY(Replicated)
    ADR_FlashDrone* FlashDrone = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "FlashDrone")
    TSubclassOf<ADR_FlashDrone> FlashDroneClass;

    UPROPERTY(EditDefaultsOnly, Category = "FlashDrone")
    float MaxDistanceFromPlayer = 700.f;
};
