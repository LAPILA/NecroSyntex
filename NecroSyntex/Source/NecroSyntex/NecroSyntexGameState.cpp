// Fill out your copyright notice in the Description page of Project Settings.

#include "NecroSyntexGameState.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"
#include "Net/UnrealNetwork.h"

ANecroSyntexGameState::ANecroSyntexGameState()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ANecroSyntexGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANecroSyntexGameState, TopScoringPlayers);
	DOREPLIFETIME(ANecroSyntexGameState, CurrentMission);
	DOREPLIFETIME(ANecroSyntexGameState, TotalPlayer);
	DOREPLIFETIME(ANecroSyntexGameState, SurvivingPlayer);
	DOREPLIFETIME(ANecroSyntexGameState, MissionCountDown);
	DOREPLIFETIME(ANecroSyntexGameState, MissionCountDownBool);
	DOREPLIFETIME(ANecroSyntexGameState, OngoingMission);
	DOREPLIFETIME(ANecroSyntexGameState, MissionRemainTime);
}

void ANecroSyntexGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (OngoingMission)
		{
			if (MissionRemainTime > 0.0f)
			{
				MissionRemainTime = FMath::Max(0.f, MissionRemainTime - DeltaTime);
			}
		}
	}

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

void ANecroSyntexGameState::PlayerDeathUpdate_Implementation()
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player DIe!"));
	SurvivingPlayer--;
	if (SurvivingPlayer <= 0)
	{
		ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();
		NecroSyntexGameMode->MissionManager->CurrentMissionFail();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Mission Fail Call!"));
		/*if (OngoingMission) {
			ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();
			NecroSyntexGameMode->MissionManager->CurrentMissionFail();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Mission Fail Call!"));
		}
		else {

		}*/
	}
}

void ANecroSyntexGameState::PlayerReviveUpdate_Implementation()
{
	SurvivingPlayer++;
}