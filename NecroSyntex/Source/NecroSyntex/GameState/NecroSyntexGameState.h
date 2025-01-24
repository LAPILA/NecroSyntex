// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NecroSyntexGameState.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ANecroSyntexGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ANecroSyntexGameState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<class ANecroSyntexGameState*> TopScoringPlayers;
private:
	float TopScore = 0.f;
};
