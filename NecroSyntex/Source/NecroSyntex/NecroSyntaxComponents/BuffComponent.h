#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NecroSyntex/HUD/BuffDebuffTypes.h"
#include "BuffComponent.generated.h"

class UDataTable;
class APlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NECROSYNTEX_API UBuffComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuffComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 외부(예: DopingComponent)에서 호출할 메인 API
    void AddBuff(FName BuffID, float Duration);
    void RemoveBuff(FName BuffID);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditDefaultsOnly, Category = "Buff")
    TObjectPtr<UDataTable> BuffDataTable;

private:
    UPROPERTY()
    TObjectPtr<APlayerCharacter> CharacterOwner;

    UPROPERTY(ReplicatedUsing = OnRep_ActiveBuffs)
    TArray<FActiveBuff> ActiveBuffs;

    UFUNCTION()
    void OnRep_ActiveBuffs();

    UFUNCTION(Server, Reliable)
    void Server_AddBuff(FName BuffID, float Duration);

    UFUNCTION(Server, Reliable)
    void Server_RemoveBuff(FName BuffID);

    void UpdateUI();
};