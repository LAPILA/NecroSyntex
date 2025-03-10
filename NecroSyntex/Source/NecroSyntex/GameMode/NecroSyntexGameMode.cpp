// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexGameMode.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "NecroSyntex/DopingSystem/DopingComponent.h"
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
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It){
		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(*It);
		UE_LOG(LogTemp, Warning, TEXT("33333"));
		if (MyPC)
		{
			MyPC->CheckPSSetTimer();
			ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();

			if (APawn* OldPawn = MyPC->GetPawn())
			{
				OldPawn->Destroy();
			}

			UE_LOG(LogTemp, Warning, TEXT("4444"));
			if (PS)
			{

				UE_LOG(LogTemp, Warning, TEXT("555555"));
				if (PS->SelectedCharacterClass)
				{
					UE_LOG(LogTemp, Warning, TEXT("66666"));
					FActorSpawnParameters SpawnParams;
					FVector SpawnVector;
					SpawnVector = FVector(-500.0f, -200.0f, 140.0f);

					APlayerCharacter* NewCharacter = GetWorld()->SpawnActor<APlayerCharacter>(
						PS->SelectedCharacterClass, SpawnVector, FRotator::ZeroRotator);
					UE_LOG(LogTemp, Warning, TEXT("777777"));
					if (NewCharacter)
					{
						if (NewCharacter->UDC)
						{
							NewCharacter->UDC->SetFirstDopingKey(PS->FirstDopingCode);
							NewCharacter->UDC->SetSecondDopingKey(PS->SecondDopingCode);
						}
						// 플레이어 컨트롤러가 새 캐릭터를 소유하도록 변경
						MyPC->Possess(NewCharacter);
						UE_LOG(LogTemp, Warning, TEXT("88888"));
						MyPC->ClientRestart(NewCharacter);
						UE_LOG(LogTemp, Warning, TEXT("99999"));
					}
				}
			}
		}
	}
}

void ANecroSyntexGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("111111"));
	ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (PC)
	{
		TotalPlayers++;
		PC->ShowCharacterSelectUI(); // 클라이언트에서 UI 띄우기
	}
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