// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimerManager.h"
#include "MissionManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMissionStart22);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMissionSuccess22);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMissionFail22);

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

	UFUNCTION()
	void ActiveMonsterSpawner();

	UFUNCTION()
	void StopMonsterSpawner();

	UFUNCTION()
	void CurrentMissionFail();

	UPROPERTY(BlueprintAssignable)
	FMissionSuccess22 MissionSuccess;

	UPROPERTY(BlueprintAssignable)
	FMissionStart22 MissionStart;

	UPROPERTY(BlueprintAssignable)
	FMissionFail22 MissionFail;

	/*UFUNCTION()
	void ActivateSpawners(FName MissionName, FName Region);*/

	UFUNCTION()
	void GameStateAndUIUpdate(bool MissionBool);


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
	void StartSurvivalMission();
	UFUNCTION()
	void SurvivalMissionSuccess();
	UFUNCTION()
	void SurvivalMissionFail();
	UFUNCTION()
	void EndSurvivlvalMission();

	FTimerHandle SurvivalTimerhandle;

	// Defense Mission
	UFUNCTION()
	void StartDefenseMission();
	UFUNCTION()
	void DefenseMissionSuccess();
	UFUNCTION()
	void DefenseMissionFail();
	UFUNCTION()
	void EndDefenseMission();

	class ADefenseTarget* CurrentDefenseTarget;

	FTimerHandle DefenseTimerHandle;

private:
	TWeakObjectPtr<class ANecroSyntexGameMode> CurrentGameMode;
};
