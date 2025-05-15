// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TimerManager.h"
#include "TestMissionTrigger.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ATestMissionTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
protected:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;


public:
	UPROPERTY()
	int32 PlayerInTrigger = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MissionDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionRegion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionName;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool OnTrigger = true;

	UPROPERTY(Replicated)
	bool OnTriggerTimer = false;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MissionTriggerActivate();

	FTimerHandle TriggerTimer;

	void TriggerDestroy();

	ATestMissionTrigger();

};
