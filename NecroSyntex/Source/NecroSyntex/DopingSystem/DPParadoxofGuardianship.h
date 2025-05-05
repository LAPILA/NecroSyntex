// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPParadoxofGuardianship.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPParadoxofGuardianship : public UDopingParent
{
	GENERATED_BODY()

private:
	UDPParadoxofGuardianship();

public:

	float DefenseBuffNum;
	float DefenseDeBuffNum;
	float WalkingBuffNum;
	float WalkingDeBuffNum;
	float RunningBuffNum;
	float RunningDeBuffNum;

	float OriPIDDefense;
	float OriPIDWalking;
	float OriPIDRunning;

	FTimerHandle ShieldCalcuBuffTimer;

	bool shieldOn;
	bool isfirst;

	void ShiledCalcu(APlayerCharacter* DopedPC);

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
