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

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
