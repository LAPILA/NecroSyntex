// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPReducePain.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPReducePain : public UDopingParent
{
	GENERATED_BODY()

public:
	float BeforeBuffDefense;
	float DefenseBuffNum;

	UDPReducePain();

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;

};
