// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPSupremeStrength.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPSupremeStrength : public UDopingParent
{
	GENERATED_BODY()

public:
	UDPSupremeStrength();

	UPROPERTY()
	float MLAttackBuffNum;
	UPROPERTY()
	float BlurredDeBuffNum;

	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;

};
