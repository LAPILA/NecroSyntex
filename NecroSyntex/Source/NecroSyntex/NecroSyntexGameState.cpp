// Fill out your copyright notice in the Description page of Project Settings.

#include "NecroSyntexGameState.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"
#include "Net/UnrealNetwork.h"

void ANecroSyntexGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANecroSyntexGameState, TopScoringPlayers);
	DOREPLIFETIME(ANecroSyntexGameState, CurrentMission);
	DOREPLIFETIME(ANecroSyntexGameState, TotalPlayer);
	DOREPLIFETIME(ANecroSyntexGameState, SurvivingPlayer);
	DOREPLIFETIME(ANecroSyntexGameState, MissionCountDown);
	DOREPLIFETIME(ANecroSyntexGameState, MissionCountDownBool);
}

void ANecroSyntexGameState::UpdateTopScore(class ANecroSyntexPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ANecroSyntexGameState::UpdateMissionStartCountdown(float newTimer)
{
	MissionCountDown = newTimer;
}