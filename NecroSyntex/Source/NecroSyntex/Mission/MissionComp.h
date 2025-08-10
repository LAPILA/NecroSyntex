// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "MissionComp.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROSYNTEX_API UMissionComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMissionComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION()
	void Init(class ANecroSyntexGameMode* InGameMode);

	UFUNCTION()
	void MissionSet(FName MissionName, FName RegionTag, float Duration);

	UPROPERTY()
	class AMissionTrigger* CMT;

	UFUNCTION()
	void CMTSet(class AMissionTrigger* MissionTrigger);

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

	UPROPERTY(EditAnywhere)
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

	// boss mission
	UFUNCTION()
	void StartBossMission();
	UFUNCTION()
	void BossMissionSuccess();
	UFUNCTION()
	void BossMissionFail();
	UFUNCTION()
	void EndBossMission();



	class ADefenseTarget* CurrentDefenseTarget;

	FTimerHandle DefenseTimerHandle;

private:
	TWeakObjectPtr<class ANecroSyntexGameMode> CurrentGameMode;
		
};
