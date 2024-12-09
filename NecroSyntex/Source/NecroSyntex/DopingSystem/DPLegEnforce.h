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


	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;
};
