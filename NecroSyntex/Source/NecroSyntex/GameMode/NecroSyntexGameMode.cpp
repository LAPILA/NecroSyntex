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
	MissionManager = CreateDefaultSubobject<UMissionComp>(TEXT("MissionManager"));
}

void ANecroSyntexGameMode::BeginPlay()
{
	Super::BeginPlay();

	//MissionManager = NewObject<UMissionComp>(this, UMissionComp::StaticClass());
	MissionManager->Init(this);


	LevelStartingTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Warning, TEXT("LevelStartingTime: %f"), LevelStartingTime);
	UE_LOG(LogTemp, Warning, TEXT("WarmUpTime: %f"), WarmUpTime);


	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginPlay 1111111111"));
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
	NecroSyntexGameState->PlayerDeathUpdate();

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RequestRespawn"));
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

		ANecroSyntexGameState* NecroSyntexGameState = GetGameState<ANecroSyntexGameState>();
		NecroSyntexGameState->PlayerReviveUpdate();

		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(ElimmedController);
		if (!MyPC) return;
		ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();
		if (PS && PS->SelectedCharacterClass)
		{
			DefaultPawnClass = PS->SelectedCharacterClass;

			if (PlayerStarts.Num() > 0)
			{
				AActor* ChosenStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
				RestartPlayerAtPlayerStart(MyPC, ChosenStart);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Player Start"));
				return;
			}

			// GetPawn() null 체크 후 안전하게 처리
			if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(MyPC->GetPawn()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Charactar Yes"));

				MyPC->ClientRestart(NewCharacter);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ClientRestart"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Player Character"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not State"));
		}

	}
}

void ANecroSyntexGameMode::SetupPlayers()
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SetupPlayers 작동 1"));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ANecroSyntexPlayerController* MyPC = Cast<ANecroSyntexPlayerController>(*It);
		if (!MyPC) continue;

		MyPC->CheckPSSetTimer();

		ANecroSyntexPlayerState* PS = MyPC->GetPlayerState<ANecroSyntexPlayerState>();

		if (APawn* OldPawn = MyPC->GetPawn())
		{
			OldPawn->Destroy();
			UE_LOG(LogTemp, Error, TEXT("캐릭터 디스트로이"));
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SetupPlayers 작동 2"));

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
				UE_LOG(LogTemp, Error, TEXT("플레이어 캐릭터 생성"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ PlayerStart 없음!"));
				continue;
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SetupPlayers 작동 3"));

			// DefaultPawnClass 원복
			//DefaultPawnClass = OldDefault;

			// GetPawn() null 체크 후 안전하게 처리
			if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(MyPC->GetPawn()))
			{
				UE_LOG(LogTemp, Warning, TEXT("777777 - 캐릭터 생성 성공"));

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


void ANecroSyntexGameMode::LevelMissionStart()
{
	MissionStart.Broadcast();
}

void ANecroSyntexGameMode::LevelMissionSuccess()
{
	MissionSuccess.Broadcast();
}

void ANecroSyntexGameMode::LevelMissionFail()
{
	MissionFail.Broadcast();
}

void ANecroSyntexGameMode::CallMissionEndEvent()
{
	MissionEndEvent.Broadcast();
}

void ANecroSyntexGameMode::SpawnNecroSyntexPlayerCharacter(APlayerController* NewPlayer)
{

	ANecroSyntexPlayerController* NSPC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (!NSPC) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("none NSPC"));
		return;
	}

	NSPC->GetInstanceAndSetSelectedCharacter();

	ANecroSyntexPlayerState* NSPS = NSPC->GetPlayerState<ANecroSyntexPlayerState>();
	if (APawn* OldPawn = NSPC->GetPawn()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("oldpawn destroy 작동"));
		OldPawn->Destroy();
	}

	if (NSPS && NSPS->SelectedCharacterClass) {

		TSubclassOf<APawn> OldDefault = DefaultPawnClass;
		DefaultPawnClass = NSPS->SelectedCharacterClass;

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		if (PlayerStarts.Num() > 0)
		{
			AActor* ChosenStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
			RestartPlayerAtPlayerStart(NSPC, ChosenStart);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerStarts Zero"));
			return;
		}


		if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(NSPC->GetPawn())) {
			NSPC->ClientRestart(NewCharacter);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NewVer ClientRestart"));
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NSPS selected character 없음"));
	}


	ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (PC)
	{
		if (ANecroSyntexGameState* GS = GetGameState<ANecroSyntexGameState>())
		{
			GS->TotalPlayer++;
			GS->SurvivingPlayer = GS->TotalPlayer;
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HandleStartingNewPlayer 작동 2"));
	}
}

void ANecroSyntexGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Post Login 작동 1"));

}

void ANecroSyntexGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HandleStartingNewPlayer 작동 1"));

	ANecroSyntexPlayerController* NSPC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (!NSPC) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("none NSPC"));
		return;
	}

	NSPC->GetInstanceAndSetSelectedCharacter();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Playerselected set 작동"));

	ANecroSyntexPlayerState* NSPS = NSPC->GetPlayerState<ANecroSyntexPlayerState>();
	if (APawn* OldPawn = NSPC->GetPawn()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("oldpawn destroy 작동"));
		OldPawn->Destroy();
	}

	if (NSPS && NSPS->SelectedCharacterClass) {

		TSubclassOf<APawn> OldDefault = DefaultPawnClass;
		DefaultPawnClass = NSPS->SelectedCharacterClass;

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		if (PlayerStarts.Num() > 0)
		{
			AActor* ChosenStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
			RestartPlayerAtPlayerStart(NSPC, ChosenStart);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerStarts Zero"));
			return;
		}


		if (APlayerCharacter* NewCharacter = Cast<APlayerCharacter>(NSPC->GetPawn())) {
			NSPC->ClientRestart(NewCharacter);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NewVer ClientRestart"));
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NSPS selected character 없음"));
	}


	ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(NewPlayer);
	if (PC)
	{
		if (ANecroSyntexGameState* GS = GetGameState<ANecroSyntexGameState>())
		{
			GS->TotalPlayer++;
			GS->SurvivingPlayer = GS->TotalPlayer;
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HandleStartingNewPlayer 작동 2"));
	}
}

void ANecroSyntexGameMode::SelectAndReadyComplete_Implementation()
{

	PlayersReadyCount++;

	CheckAllPlayersReady();

}

void ANecroSyntexGameMode::CheckAllPlayersReady()
{
	if (ANecroSyntexGameState* GS = GetGameState<ANecroSyntexGameState>())
	{
		if (PlayersReadyCount >= GS->PlayerArray.Num()) {
			SetupPlayers();
		}
	}

}