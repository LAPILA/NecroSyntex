// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectGameMode.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"


ACharacterSelectGameMode::ACharacterSelectGameMode()
{
	PlayersReadyCount = 0; // �ʱ�ȭ
    bUseSeamlessTravel = true;
}

void ACharacterSelectGameMode::BeginPlay()
{
	Super::BeginPlay();

	//ShowCharacterSelectionUI();
}

void ACharacterSelectGameMode::ShowCharacterSelectionUI()
{
    // ���� ������ ��� �÷��̾� ��Ʈ�ѷ� ��������
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ANecroSyntexPlayerController * PC = Cast<ANecroSyntexPlayerController>(*It);
        if (PC)
        {
            PC->ShowCharacterSelectUI(); // Ŭ���̾�Ʈ���� UI ����
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