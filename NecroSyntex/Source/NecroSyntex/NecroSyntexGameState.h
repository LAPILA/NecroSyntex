// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NecroSyntexGameState.generated.h"

/**
 * // NOTICE : when you want to handle about game, fix it
 */
UCLASS()
class NECROSYNTEX_API ANecroSyntexGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ANecroSyntexPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<ANecroSyntexPlayerState*> TopScoringPlayers;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FString CurrentMission;
private:

	float TopScore = 0.f;
};
