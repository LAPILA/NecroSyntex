
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SubComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROSYNTEX_API USubComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    USubComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ------------------ Health ------------------
    void Heal(float HealAmount, float HealingTime);

private:
    void HealRampUp(float DeltaTime);

    bool bHealing = false;
    UPROPERTY(EditAnywhere, Category = "HealthHeal")
    float HealingRate = 3.f;
    UPROPERTY(EditAnywhere, Category = "HealthHeal")
    float AmountToHeal = 100.f;

    // ------------------ Shield ------------------
public:
    void OnTakeDamage();

private:
    void ShieldRampUp(float DeltaTime);

    float LastDamageTime = 0.f;
    UPROPERTY(EditAnywhere, Category = "ShieldHeal")
    float ShieldRegenDelay = 3.f;
    UPROPERTY(EditAnywhere, Category = "ShieldHeal")
    float ShieldRegenRate = 100.f;

    class APlayerCharacter* Character = nullptr;
};