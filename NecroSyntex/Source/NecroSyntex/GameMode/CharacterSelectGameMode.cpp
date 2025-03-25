// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectGameMode.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"


ACharacterSelectGameMode::ACharacterSelectGameMode()
{
	PlayersReadyCount = 0; // 초기화
    bUseSeamlessTravel = true;
}

void ACharacterSelectGameMode::BeginPlay()
{
	Super::BeginPlay();

	//ShowCharacterSelectionUI();
}

void ACharacterSelectGameMode::ShowCharacterSelectionUI()
{
    // 현재 접속한 모든 플레이어 컨트롤러 가져오기
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ANecroSyntexPlayerController * PC = Cast<ANecroSyntexPlayerController>(*It);
        if (PC)
        {
            PC->ShowCharacterSelectUI(); // 클라이언트에서 UI 띄우기
        }
    }
}

void ACharacterSelectGameMode::SelectAndReadyComplete_Implementation()
{

    PlayersReadyCount++;

    CheckAllPlayersReady();

}

void ACharacterSelectGameMode::CheckAllPlayersReady()
{
    if (PlayersReadyCount >= TotalPlayers) {
        StartMainGame();
    }
}

void ACharacterSelectGameMode::StartMainGame()
{
    GetWorld()->ServerTravel("/Game/MainContents/Blueprints/CharacterSelect/TestServerTravelSecond?listen");

}

void ACharacterSelectGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ShowCharacterSelectionUI();
}