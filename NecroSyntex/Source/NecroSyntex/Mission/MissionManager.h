// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MissionManager.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UMissionManager : public UObject
{
	GENERATED_BODY()

public:
	void Init(class ANecroSyntexGameMode* InGameMode);

	void StartSurvivalMission(FName RegionTag, float Duration);
	void EndSurvivlvalMission(FName RegionTag);

	void ActivateSpawners(FName Region);



private:
	TWeakObjectPtr<class ANecroSyntexGameMode> CurrentGameMode;
};
