// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPForcedHealing.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDPForcedHealing : public UDopingParent
{
	GENERATED_BODY()

public:

	float targetRecover;
	float BuffRecoverAPS;
	float DebuffMaxHP;
	UDPForcedHealing();

	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;

};
