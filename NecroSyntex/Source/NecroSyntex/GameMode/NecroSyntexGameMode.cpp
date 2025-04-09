// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexGameMode.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "NecroSyntex\NecroSyntexGameState.h"
#include "NecroSyntex\PlayerState\NecroSyntexPlayerState.h"
#include "NecroSyntex\DopingSystem\DopingComponent.h"

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

	MissionManager = NewObject<UMissionManager>(this, UMissionManager::StaticClass());
	MissionManager->Init(this);


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


//�÷��̾� ���� ���� �Լ�
void ANecroSyntexGameMode::PlayerEliminated(APlayerCharacter* ElimmedCharacter, ANecroSyntexPlayerController* VictimController, ANecroSyntexPlayerController* AttackController)
{
	// ������, ����� State ���� Ȯ��
	ANecroSyntexPlayerState* AttackerPlayerState = AttackController ? Cast<ANecroSyntexPlayerState>(AttackController->PlayerState) : nullptr;
	ANecroSyntexPlayerState* VictimPlayerState = VictimController ? Cast<ANecroSyntexPlayerState>(VictimController->PlayerState) : nullptr;

	// ���� �߰�
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

void ANecroSyntexGameMode::SetupPlayers()
{
	UE_LOG(LogTemp, Warning, TEXT("22222"));

	// ��� PlayerController ��ȸ
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(*It);
		if (!MyPC) continue;

		UE_LOG(LogTemp, Warning, TEXT("33333"));
		MyPC->CheckPSSetTimer();

		// PlayerState ��������
		ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();

		// ���� Pawn(�ӽ� ĳ����)�� �����
		if (APawn* OldPawn = MyPC->GetPawn())
		{
			OldPawn->Destroy();
		}

		UE_LOG(LogTemp, Warning, TEXT("4444"));
		if (PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("555555"));

			// �÷��̾ ������ ĳ���� Ŭ������ ������ ���� ����
			if (PS->SelectedCharacterClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("66666"));

				// 1) ���� DefaultPawnClass ����
				TSubclassOf<APawn> OldDefault = DefaultPawnClass;

				// 2) DefaultPawnClass�� �÷��̾ ������ Ŭ������ �ӽ� ����
				DefaultPawnClass = PS->SelectedCharacterClass;

				// 3) RestartPlayer() �� ���������� PlayerStart�� ã�� Spawn & Possess
				RestartPlayer(MyPC);

				// �ٽ� ���� ����
				DefaultPawnClass = OldDefault;

				// Spawn�� �� ĳ����(= MyPC->GetPawn())���� DopingComponent ���� ����
				if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(MyPC->GetPawn()))
				{
					UE_LOG(LogTemp, Warning, TEXT("777777"));

					if (NewCharacter->UDC)
					{
						NewCharacter->UDC->SetFirstDopingKey(PS->FirstDopingCode);
						NewCharacter->UDC->SetSecondDopingKey(PS->SecondDopingCode);
					}

					UE_LOG(LogTemp, Warning, TEXT("88888"));
					// �Ʒ� ClientRestart ȣ���� �ʼ��� �ƴ�����,
					// ���� ������ UI/State �絿��ȭ�� �ʿ��ϴٸ� ����
					MyPC->ClientRestart(NewCharacter);

					UE_LOG(LogTemp, Warning, TEXT("99999"));
				}
			}
		}
	}
}


void ANecroSyntexGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//UE_LOG(LogTemp, Warning, TEXT("111111"));
	//ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	//if (PC)
	//{
	//	TotalPlayers++;
	//	PC->ShowCharacterSelectUI(); // Ŭ���̾�Ʈ���� UI ����
	//}
}

void ANecroSyntexGameMode::SelectAndReadyComplete_Implementation()
{

	PlayersReadyCount++;

	CheckAllPlayersReady();

}

void ANecroSyntexGameMode::ShowCharacterSelectionUI()
{
	// ���� ������ ��� �÷��̾� ��Ʈ�ѷ� ��������
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(*It);
		if (PC)
		{
			PC->ShowCharacterSelectUI(); // Ŭ���̾�Ʈ���� UI ����
		}
	}
}

void ANecroSyntexGameMode::CheckAllPlayersReady()
{
	if (PlayersReadyCount >= TotalPlayers) {
		SetupPlayers();
	}
}