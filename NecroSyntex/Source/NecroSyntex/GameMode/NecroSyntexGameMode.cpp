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

	// 모든 PlayerController 순회
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(*It);
		if (!MyPC) continue;

		UE_LOG(LogTemp, Warning, TEXT("33333"));
		MyPC->CheckPSSetTimer();

		// PlayerState 가져오기
		ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();

		// 기존 Pawn(임시 캐릭터)을 지우기
		if (APawn* OldPawn = MyPC->GetPawn())
		{
			OldPawn->Destroy();
		}

		UE_LOG(LogTemp, Warning, TEXT("4444"));
		if (PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("555555"));

			// 플레이어가 선택한 캐릭터 클래스가 있으면 스폰 진행
			if (PS->SelectedCharacterClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("66666"));

				// 1) 기존 DefaultPawnClass 저장
				TSubclassOf<APawn> OldDefault = DefaultPawnClass;

				// 2) DefaultPawnClass를 플레이어가 선택한 클래스로 임시 변경
				DefaultPawnClass = PS->SelectedCharacterClass;

				// 3) RestartPlayer() → 내부적으로 PlayerStart를 찾아 Spawn & Possess
				RestartPlayer(MyPC);

				// 다시 원상 복귀
				DefaultPawnClass = OldDefault;

				// Spawn된 새 캐릭터(= MyPC->GetPawn())에서 DopingComponent 등을 설정
				if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(MyPC->GetPawn()))
				{
					UE_LOG(LogTemp, Warning, TEXT("777777"));

					if (NewCharacter->UDC)
					{
						NewCharacter->UDC->SetFirstDopingKey(PS->FirstDopingCode);
						NewCharacter->UDC->SetSecondDopingKey(PS->SecondDopingCode);
					}

					UE_LOG(LogTemp, Warning, TEXT("88888"));
					// 아래 ClientRestart 호출은 필수가 아니지만,
					// 별도 이유로 UI/State 재동기화가 필요하다면 유지
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
	if (PlayersReadyCount >= TotalPlayers) {
		SetupPlayers();
	}
}