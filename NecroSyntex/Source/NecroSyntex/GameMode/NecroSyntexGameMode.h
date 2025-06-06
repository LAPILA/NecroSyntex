// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NecroSyntex/Mission/MissionManager.h"
#include "NecroSyntexGameMode.generated.h"

namespace MatchState
{
	extern NECROSYNTEX_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

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
	float WarmUpTime = 1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 180.f; //게임 시간 컨트롤 가능.

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;


public:

	//������ ���� ����
	 // ��� �÷��̾�� ĳ���� ���� UI�� ǥ��
	void ShowCharacterSelectionUI();

	// �÷��̾ ĳ���͸� �����ϸ� ������ �˸��� �Լ�
	UFUNCTION(Server, Reliable)
	void SelectAndReadyComplete();

	// ��� �÷��̾ ������ �Ϸ��ߴ��� Ȯ��
	void CheckAllPlayersReady();


public:
	// ������ �Ϸ��� �÷��̾� ��
	int32 PlayersReadyCount = 0;

	// �� �÷��̾� ��
	int32 TotalPlayers = 0; // �κ񿡼� ������ �� ����

	FTimerHandle CheckPlayerStateTimer;

	void SetupPlayers();

	UPROPERTY()
	FVector SpawnLocation;
	FVector SpawnRotation;


	// Mission
	UPROPERTY(BlueprintReadWrite)
	UMissionManager* MissionManager;


protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
