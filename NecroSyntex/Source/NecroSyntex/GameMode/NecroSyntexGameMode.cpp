// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexGameMode.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"

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
			UE_LOG(LogTemp, Warning, TEXT("Notifying PlayerController: %s"), *NecroSyntexPlayer->GetName());
			NecroSyntexPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void ANecroSyntexGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmUpTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			StartMatch();
			UE_LOG(LogTemp, Warning, TEXT("Match started! State: %s"), *MatchState.ToString());
		}
	}
}

//�÷��̾� ���� ���� �Լ�
void ANecroSyntexGameMode::PlayerEliminated(APlayerCharacter* ElimmedCharacter, ANecroSyntexPlayerController* VictimController, ANecroSyntexPlayerController* AttackController)
{
	// ������, ����� State ���� Ȯ��
	ANecroSyntexPlayerState* AttackerPlayerState = AttackController ? Cast<ANecroSyntexPlayerState>(AttackController->PlayerState) : nullptr;
	ANecroSyntexPlayerState* VictimPlayerState = VictimController ? Cast<ANecroSyntexPlayerState>(VictimController->PlayerState) : nullptr;

	// ���� �߰�
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(-100.f);
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
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

