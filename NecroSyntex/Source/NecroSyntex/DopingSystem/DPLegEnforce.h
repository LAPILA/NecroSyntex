// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPLegEnforce.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPLegEnforce : public UDopingParent
{
	GENERATED_BODY()

public:
	UDPLegEnforce();

	float WalkingBuffNum;
	float RunningBuffNum;
	float WalkingDeBuffNum;
	float RunningDeBuffNum;


	bool UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

	virtual bool GetBuff() const override { return  CheckBuff; };
	virtual bool GetDeBuff() const override { return CheckDeBuff; };
};
