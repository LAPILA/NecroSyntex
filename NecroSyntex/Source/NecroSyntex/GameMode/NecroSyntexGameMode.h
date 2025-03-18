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


public:

	//박태혁 편집 구간
	 // 모든 플레이어에게 캐릭터 선택 UI를 표시
	void ShowCharacterSelectionUI();

	// 플레이어가 캐릭터를 선택하면 서버에 알리는 함수
	UFUNCTION(Server, Reliable)
	void SelectAndReadyComplete();

	// 모든 플레이어가 선택을 완료했는지 확인
	void CheckAllPlayersReady();


public:
	// 선택을 완료한 플레이어 수
	int32 PlayersReadyCount = 0;

	// 총 플레이어 수
	int32 TotalPlayers = 1; // 로비에서 설정될 수 있음

	FTimerHandle CheckPlayerStateTimer;

	void SetupPlayers();

	UPROPERTY()
	FVector SpawnLocation;
	FVector SpawnRotation;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
