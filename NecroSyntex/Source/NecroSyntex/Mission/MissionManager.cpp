// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/Monster/M_Spawner.h"
#include "TimerManager.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Character/PlayerCharacter.h"

void UMissionManager::Init(class ANecroSyntexGameMode* InGameMode)
{
	CurrentGameMode = InGameMode;
}

void UMissionManager::StartSurvivalMission(FName RegionTag, float Duration)
{
    // ���� �̼� ���� ������ �۵�
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == RegionTag)
        {
            Spawner->StartSpawnMonster(Spawner->MonsterSpawnSpeed);
        }
    }

    // GameState
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->CurrentMission = "Survival";
    }

    // �÷��̾� �̼� UI ������Ʈ
    PlayerMissionUIUpdate("Survival");

    // ���� �̼� Ÿ�̸� �۵�
    FTimerHandle handle;
    GetWorld()->GetTimerManager().SetTimer(
        handle,
        [this, RegionTag]() { EndSurvivlvalMission(RegionTag); },
        Duration,
        false
    );
}

void UMissionManager::EndSurvivlvalMission(FName RegionTag)
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == RegionTag)
        {
            Spawner->StopSpawnMonster();
        }
    }
}

void UMissionManager::PlayerMissionUIUpdate(FString MissionName)
{
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundPlayers);

    for (AActor* Actor : FoundPlayers)
    {
        APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
        if (Player)
        {
            Player->MissionUIUpdate();
        }
    }
}