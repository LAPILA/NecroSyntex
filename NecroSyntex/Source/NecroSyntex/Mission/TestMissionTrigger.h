// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
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

	void PlayerTriggerOverlap(AActor* OtherActor);

public:
	UPROPERTY()
	int32 PlayerInTrigger = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MissionDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionRegion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionName;



	ATestMissionTrigger();

};
