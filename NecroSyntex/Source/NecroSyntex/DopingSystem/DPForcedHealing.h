// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPForcedHealing.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPForcedHealing : public UDopingParent
{
	GENERATED_BODY()

public:

	float targetRecover;
	float BuffRecoverAPS;
	float DebuffMaxHP;
	UDPForcedHealing();

	FTimerHandle HealingTimer;

	void HealCharacter(APlayerCharacter* DopedPC);

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
