// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CharacterSelectGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ACharacterSelectGameMode : public AGameMode
{
	GENERATED_BODY()

public:
    ACharacterSelectGameMode();

    // 모든 플레이어에게 캐릭터 선택 UI를 표시
    void ShowCharacterSelectionUI();

    // 플레이어가 캐릭터를 선택하면 서버에 알리는 함수
    UFUNCTION(Server, Reliable)
    void SelectAndReadyComplete();

    // 모든 플레이어가 선택을 완료했는지 확인
    void CheckAllPlayersReady();

    // 모든 플레이어가 선택을 완료하면 메인 게임 레벨로 이동
    void StartMainGame();

protected:
    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

private:
    // 선택을 완료한 플레이어 수
    int32 PlayersReadyCount = 0;

    // 총 플레이어 수
    int32 TotalPlayers = 1; // 로비에서 설정될 수 있음
	
};
