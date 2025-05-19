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

	bool UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
