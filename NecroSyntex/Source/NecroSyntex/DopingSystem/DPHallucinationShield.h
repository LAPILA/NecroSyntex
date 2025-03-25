// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPHallucinationShield.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDPHallucinationShield : public UDopingParent
{
	GENERATED_BODY()
	
private:
	UDPHallucinationShield();

public:

	UFUNCTION(BlueprintCallable)
	void CallCameraShake();

	void UseDopingItem(APlayerCharacter* DopedPC) override;

	void BuffOn(APlayerCharacter* DopedPC) override;

	void DeBuffOn(APlayerCharacter* DopedPC) override;

	void BuffOff(APlayerCharacter* DopedPC) override;

	void DeBuffOff(APlayerCharacter* DopedPC) override;
};
