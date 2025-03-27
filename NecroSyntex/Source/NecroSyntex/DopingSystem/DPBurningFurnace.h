// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPBurningFurnace.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPBurningFurnace : public UDopingParent
{
	GENERATED_BODY()

public:
	UDPBurningFurnace();

	float DefenseBuffNum;

	FTimerHandle DamageTimer;
	FTimerHandle PlayerHPMinusTimer;

	void BFDamageApply(APlayerCharacter* DopedPC);

	void PCHPMinus(APlayerCharacter* DopedPC);

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
