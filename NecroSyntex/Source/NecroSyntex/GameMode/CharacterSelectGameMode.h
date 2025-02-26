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

    // ��� �÷��̾�� ĳ���� ���� UI�� ǥ��
    void ShowCharacterSelectionUI();

    // �÷��̾ ĳ���͸� �����ϸ� ������ �˸��� �Լ�
    UFUNCTION(Server, Reliable)
    void SelectAndReadyComplete();

    // ��� �÷��̾ ������ �Ϸ��ߴ��� Ȯ��
    void CheckAllPlayersReady();

    // ��� �÷��̾ ������ �Ϸ��ϸ� ���� ���� ������ �̵�
    void StartMainGame();

protected:
    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

private:
    // ������ �Ϸ��� �÷��̾� ��
    int32 PlayersReadyCount = 0;

    // �� �÷��̾� ��
    int32 TotalPlayers = 1; // �κ񿡼� ������ �� ����
	
};
