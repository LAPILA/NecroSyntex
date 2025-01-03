// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NecroSyntexGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ANecroSyntexGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ANecroSyntexGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class APlayerCharacter* ElimmedCharacter, class ANecroSyntexPlayerController* VictimController, class ANecroSyntexPlayerController* AttackController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;
};
