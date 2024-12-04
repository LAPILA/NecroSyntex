// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Mode")
	int32 RequiredNumberOfPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Mode")
	FString DestinationMapPath;
};
