
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
	friend class APlayerCharacter;
	void Heal(float HealAmount, float HealingTime);
protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);

private:
	UPROPERTY()
	class APlayerCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
