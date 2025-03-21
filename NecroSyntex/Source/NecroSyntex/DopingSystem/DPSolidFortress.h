// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPSolidFortress.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPSolidFortress : public UDopingParent
{
	GENERATED_BODY()

public:
	UDPSolidFortress();

	UPROPERTY()
	float HPBuffNum;
	UPROPERTY()
	float WalkingDeBuffNum;
	UPROPERTY()
	float RunningDeBuffNum;

	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;

};
