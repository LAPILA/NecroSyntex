// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPCurseofChaos.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDPCurseofChaos : public UDopingParent
{
	GENERATED_BODY()

private:
	UDPCurseofChaos();
	

public:
	float WalkingBuffNum;
	float RunningBuffNum;
	float BuffRecoverAPS;

	FTimerHandle HealingTimer;

	void HealCharacter(UPlayerInformData* PID);

	void UseDopingItem(UPlayerInformData* PID) override;

	void BuffOn(UPlayerInformData* PID) override;

	void DeBuffOn(UPlayerInformData* PID) override;

	void BuffOff(UPlayerInformData* PID) override;

	void DeBuffOff(UPlayerInformData* PID) override;
};
