// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NecroSyntexGameState.generated.h"

/**
 * // NOTICE : when you want to handle about game, fix it
 */
UCLASS()
class NECROSYNTEX_API ANecroSyntexGameState : public AGameState
{
	GENERATED_BODY()

private:
	ANecroSyntexGameState();

protected:
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ANecroSyntexPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<ANecroSyntexPlayerState*> TopScoringPlayers;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName CurrentMission = " ";

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName CurrentRegion = " ";

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool OngoingMission = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MissionSequence = 0;  // zero is first mission state



	// Mission

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TotalPlayer = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 SurvivingPlayer = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float MissionCountDown;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float MissionRemainTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool MissionCountDownBool = false;

	void UpdateMissionStartCountdown(float newTimer);

	UFUNCTION(Server, Reliable)
	void PlayerDeathUpdate();

	UFUNCTION(Server, Reliable)
	void PlayerReviveUpdate();


private:

	float TopScore = 0.f;
};
