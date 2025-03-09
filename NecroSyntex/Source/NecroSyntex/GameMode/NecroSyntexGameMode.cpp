// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexGameMode.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "NecroSyntex\NecroSyntexGameState.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ANecroSyntexGameMode::ANecroSyntexGameMode()
{
	//Delay mode (if you want, delete)
	bDelayedStart = true;

}

void ANecroSyntexGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Warning, TEXT("LevelStartingTime: %f"), LevelStartingTime);
	UE_LOG(LogTemp, Warning, TEXT("WarmUpTime: %f"), WarmUpTime);
}

void ANecroSyntexGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* NecroSyntexPlayer = Cast<ANecroSyntexPlayerController>(*It);
		if (NecroSyntexPlayer)
		{
			NecroSyntexPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void ANecroSyntexGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmUpTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime <= 0.0f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmUpTime + MatchTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmUpTime + MatchTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}


//플레이어 제거 관련 함수
void ANecroSyntexGameMode::PlayerEliminated(APlayerCharacter* ElimmedCharacter, ANecroSyntexPlayerController* VictimController, ANecroSyntexPlayerController* AttackController)
{
	// 공격자, 희생자 State 존재 확인
	ANecroSyntexPlayerState* AttackerPlayerState = AttackController ? Cast<ANecroSyntexPlayerState>(AttackController->PlayerState) : nullptr;
	ANecroSyntexPlayerState* VictimPlayerState = VictimController ? Cast<ANecroSyntexPlayerState>(VictimController->PlayerState) : nullptr;

	// 점수 추가
	ANecroSyntexGameState* NecroSyntexGameState = GetGameState<ANecroSyntexGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && NecroSyntexGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		NecroSyntexGameState->UpdateTopScore(AttackerPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ANecroSyntexGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ElimmedController valid"))
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

