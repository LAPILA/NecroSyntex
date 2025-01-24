// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexGameState.h"
#include "Net\UnrealNetwork.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"

void ANecroSyntexGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANecroSyntexGameState, TopScoringPlayers);

}

void ANecroSyntexGameState::UpdateTopScore(class ANecroSyntexGameState* ScoringPlayer)
{

	//To Do: Fix Score System (if it can't work... remove all score system.. 
	/*
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->AddToScore();
	}
	else if (ScoringPlayer->AddToScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->AddToScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->AddToScore();
	}
	*/
}