// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPFinalEmber.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPFinalEmber : public UDopingParent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float HPBuffNum;
	UPROPERTY()
	float WalkingBuffNum;
	UPROPERTY()
	float RunningBuffNum;
	UPROPERTY()
	float MLAttackBuffNum;


	UDPFinalEmber();

	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;

};
