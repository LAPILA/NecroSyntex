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
		NecroSyntexGameState->PlayerDeathUpdate();
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

		ANecroSyntexGameState* NecroSyntexGameState = GetGameState<ANecroSyntexGameState>();
		NecroSyntexGameState->PlayerReviveUpdate();

		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);


	}
}

void ANecroSyntexGameMode::SetupPlayers()
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(*It);
		if (!MyPC) continue;

		MyPC->CheckPSSetTimer();

		ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();

		if (APawn* OldPawn = MyPC->GetPawn())
		{
			OldPawn->Destroy();
		}


		if (PS && PS->SelectedCharacterClass)
		{

			// DefaultPawnClass를 변경
			TSubclassOf<APawn> OldDefault = DefaultPawnClass;
			DefaultPawnClass = PS->SelectedCharacterClass;

			// PlayerStart 직접 선택 (안전하게 처리)
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

			if (PlayerStarts.Num() > 0)
			{
				AActor* ChosenStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
				RestartPlayerAtPlayerStart(MyPC, ChosenStart);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ PlayerStart 없음!"));
				continue;
			}

			// DefaultPawnClass 원복
			DefaultPawnClass = OldDefault;

			// GetPawn() null 체크 후 안전하게 처리
			if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(MyPC->GetPawn()))
			{
				UE_LOG(LogTemp, Warning, TEXT("777777 - 캐릭터 생성 성공"));

				/*if (NewCharacter->UDC)
				{
					NewCharacter->UDC->SetFirstDopingKey(PS->FirstDopingCode);
					NewCharacter->UDC->SetSecondDopingKey(PS->SecondDopingCode);
				}*/

				MyPC->ClientRestart(NewCharacter);
				UE_LOG(LogTemp, Warning, TEXT("99999 - ClientRestart 호출"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ 캐릭터 스폰 실패! GetPawn() == nullptr"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ PlayerState 또는 SelectedCharacterClass가 유효하지 않음"));
		}
	}
}


void ANecroSyntexGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (PC)
	{
		if (ANecroSyntexGameState* GS = GetGameState<ANecroSyntexGameState>())
		{
			GS->TotalPlayer++;
			GS->SurvivingPlayer = GS->TotalPlayer;
		}

		PC->GetInstanceAndSetSelectedCharacter();
		SelectAndReadyComplete();
	}

	//UE_LOG(LogTemp, Warning, TEXT("111111"));
	//ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	//if (PC)
	//{
	//	TotalPlayers++;
	//	PC->ShowCharacterSelectUI(); // 클라이언트에서 UI 띄우기
	//}
}

void ANecroSyntexGameMode::SelectAndReadyComplete_Implementation()
{

	PlayersReadyCount++;

	CheckAllPlayersReady();

}

void ANecroSyntexGameMode::ShowCharacterSelectionUI()
{
	// 현재 접속한 모든 플레이어 컨트롤러 가져오기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(*It);
		if (PC)
		{
			PC->ShowCharacterSelectUI(); // 클라이언트에서 UI 띄우기
		}
	}
}

void ANecroSyntexGameMode::CheckAllPlayersReady()
{
	if (ANecroSyntexGameState* GS = GetGameState<ANecroSyntexGameState>())
	{
		if (PlayersReadyCount >= GS->TotalPlayer) {
			SetupPlayers();
		}
	}

}