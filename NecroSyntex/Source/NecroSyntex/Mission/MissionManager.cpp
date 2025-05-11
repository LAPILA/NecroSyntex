// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/Monster/M_Spawner.h"
#include "NecroSyntex/Mission/DefenseTarget.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void UMissionManager::Init(class ANecroSyntexGameMode* InGameMode)
{
	CurrentGameMode = InGameMode;
    count = 3.0f;
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
        StartSurvivalMission();
    }
    else if (CurrentMissionName == "Defense") {
        StartDefenseMission();
    }
    else if (CurrentMissionName == "Boss") {

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("MissionName is Unavilable"));
    }
}

void UMissionManager::ActiveMonsterSpawner()
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == CurrentRegionTag && Spawner->MissionName == CurrentMissionName)
        {
            Spawner->StartSpawnMonster(Spawner->MonsterSpawnSpeed);
        }
    }
}

void UMissionManager::StopMonsterSpawner()
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AM_Spawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        AM_Spawner* Spawner = Cast<AM_Spawner>(Actor);
        if (Spawner && Spawner->RegionTag == CurrentRegionTag && Spawner->MissionName == CurrentMissionName)
        {
            Spawner->StopSpawnMonster();
        }
    }
}

void UMissionManager::CurrentMissionFail()
{
    if (CurrentMissionName == "Survival") {
        SurvivalMissionFail();
    }
    else if (CurrentMissionName == "Defense") {
        DefenseMissionFail();
    }
    else if (CurrentMissionName == "Boss") {

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("MissionName is Unavilable"));
    }
}


//Survival Mission
void UMissionManager::StartSurvivalMission()
{
    // 생존 미션 몬스터 스포너 작동
    ActiveMonsterSpawner();

    GameStateAndUIUpdate(true);

    MissionStart.Broadcast();

    // 생존 미션 타이머 작동
    GetWorld()->GetTimerManager().SetTimer(
        SurvivalTimerhandle,
        [this]() { SurvivalMissionSuccess(); },
        CurrentDuration,
        false
    );
}

void UMissionManager::SurvivalMissionSuccess()
{
    MissionSuccess.Broadcast();

    EndSurvivlvalMission();

}

void UMissionManager::SurvivalMissionFail()
{
    GetWorld()->GetTimerManager().ClearTimer(SurvivalTimerhandle);

    EndSurvivlvalMission();

    MissionFail.Broadcast();
}

void UMissionManager::EndSurvivlvalMission()
{
    StopMonsterSpawner();

    MissionSet("None", "None", 0.0f);

    GameStateAndUIUpdate(false);
}


//DefenseMission

void UMissionManager::StartDefenseMission()
{
    TArray<AActor*> FoundDefenseTarget;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseTarget::StaticClass(), FoundDefenseTarget);

    for (AActor* Actor : FoundDefenseTarget)
    {
        ADefenseTarget* DefenseTarget = Cast<ADefenseTarget>(Actor);
        if (DefenseTarget && DefenseTarget->RegionName == CurrentRegionTag)
        {
            DefenseTarget->DefenseObjectActive();
        }
    }

    ActiveMonsterSpawner();

    GameStateAndUIUpdate(true);

    MissionStart.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        DefenseTimerHandle,
        [this]() { DefenseMissionSuccess(); },
        CurrentDuration,
        false
    );
}

void UMissionManager::DefenseMissionSuccess()
{
    MissionSuccess.Broadcast();

    EndSurvivlvalMission();
}

void UMissionManager::DefenseMissionFail()
{
    GetWorld()->GetTimerManager().ClearTimer(DefenseTimerHandle);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Mission Fail"));
    EndSurvivlvalMission();

    MissionFail.Broadcast();
}

void UMissionManager::EndDefenseMission()
{
    StopMonsterSpawner();

    MissionSet("None", "None", 0.0f);

    GameStateAndUIUpdate(false);
}

//Game State Update
void UMissionManager::GameStateAndUIUpdate( bool MissionBool)
{
    // GameState && Auto Mission UI Update
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->CurrentMission = CurrentMissionName;
        GS->CurrentRegion = CurrentRegionTag;
        GS->MissionRemainTime = CurrentDuration;
        GS->OngoingMission = MissionBool;
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