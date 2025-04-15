// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/Monster/M_Spawner.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Character/PlayerCharacter.h"

void UMissionManager::Init(class ANecroSyntexGameMode* InGameMode)
{
	CurrentGameMode = InGameMode;
    count = 10.0f;
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->SurvivingPlayer = GS->TotalPlayer;
    }
}

void UMissionManager::MissionSet(FName MissionName, FName RegionTag, float Duration)
{
    CurrentMissionName = MissionName;
    CurrentRegionTag = RegionTag;
    CurrentDuration = Duration;
}

void UMissionManager::ActiveMission()
{
    if (CurrentMissionName == "Survival") {
        StartSurvivalMission(CurrentMissionName, CurrentRegionTag, CurrentDuration);
    }
    else if (CurrentMissionName == "Defense") {

    }
    else if (CurrentMissionName == "Boss") {

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("MissionName is Unavilable"));
    }
}

void UMissionManager::StartSurvivalMission(FName MissionName, FName RegionTag, float Duration)
{
    // 생존 미션 몬스터 스포너 작동
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == RegionTag && Spawner->MissionName == MissionName)
        {
            Spawner->StartSpawnMonster(Spawner->MonsterSpawnSpeed);
        }
    }

    GameStateAndUIUpdate("Survival");

    // 생존 미션 타이머 작동
    FTimerHandle handle;
    GetWorld()->GetTimerManager().SetTimer(
        handle,
        [this, MissionName, RegionTag]() { EndSurvivlvalMission(MissionName, RegionTag); },
        Duration,
        false
    );
}

void UMissionManager::EndSurvivlvalMission(FName MissionName, FName RegionTag)
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == RegionTag && Spawner->MissionName == MissionName)
        {
            Spawner->StopSpawnMonster();
        }
    }

    GameStateAndUIUpdate(" ");
}

void UMissionManager::GameStateAndUIUpdate(FName MissionName)
{
    // GameState && Auto Mission UI Update
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->CurrentMission = MissionName;
    }
}

void UMissionManager::MissionCountdownStart()
{
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    GS->MissionCountDown = count;
    GS->MissionCountDownBool = true;

    GetWorld()->GetTimerManager().SetTimer(
        CountDownTimer,
        [this]() { UpdateMissionCountdown(); },
        1.0f,
        true
    );
}

void UMissionManager::UpdateMissionCountdown()
{
    if (ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState()))
    {
        GS->UpdateMissionStartCountdown(GS->MissionCountDown - 1.f);

        if (GS->MissionCountDown <= 0)
        {
            GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
            GS->MissionCountDownBool = false;
            ActiveMission(); // 실제 미션 시작
        }
    }
}

void UMissionManager::MissionCountdownCancel()
{
    GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
    if (ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState())) {
        GS->MissionCountDownBool = false;
    }
}