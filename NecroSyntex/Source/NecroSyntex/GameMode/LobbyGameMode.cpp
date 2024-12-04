// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
ALobbyGameMode::ALobbyGameMode()
{
    RequiredNumberOfPlayers = 1;
    DestinationMapPath = TEXT("/Game/MainContents/Maps/TestMainMap");
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    AGameStateBase* GameStateBase = GetGameState<AGameStateBase>();
    if (!GameStateBase)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameStateBase is null"));
        return;
    }

    int32 NumberOfPlayers = GameStateBase->PlayerArray.Num();
    if (NumberOfPlayers >= RequiredNumberOfPlayers)
    {
        UWorld* World = GetWorld();
        if (!World)
        {
            UE_LOG(LogTemp, Warning, TEXT("World is null"));
            return;
        }

        bUseSeamlessTravel = true;
        World->ServerTravel(DestinationMapPath);
    }
}