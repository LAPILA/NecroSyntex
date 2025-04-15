// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimerManager.h"
#include "MissionManager.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UMissionManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Init(class ANecroSyntexGameMode* InGameMode);

	UFUNCTION()
	void MissionSet(FName MissionName, FName RegionTag, float Duration);

	UPROPERTY()
	FName CurrentMissionName;
	UPROPERTY()
	FName CurrentRegionTag;
	UPROPERTY()
	float CurrentDuration;

	UFUNCTION()
	void ActiveMission();

	/*UFUNCTION()
	void ActivateSpawners(FName MissionName, FName Region);*/

	UFUNCTION()
	void GameStateAndUIUpdate(FName MissionName);


	//count down
	UFUNCTION()
	void MissionCountdownStart();
	
	UFUNCTION()
	void MissionCountdownCancel();

	UFUNCTION()
	void UpdateMissionCountdown();

	UPROPERTY()
	float count;

	FTimerHandle CountDownTimer;

	// Survival Mission
	UFUNCTION()
	void StartSurvivalMission(FName MissionName, FName RegionTag, float Duration);
	/*UFUNCTION()
	void SurvivalMissionSuccess();
	UFUNCTION()
	void SurvivalMissionFail();*/
	UFUNCTION()
	void EndSurvivlvalMission(FName MissionName, FName RegionTag);

	// Defense Mission

private:
	TWeakObjectPtr<class ANecroSyntexGameMode> CurrentGameMode;
};
