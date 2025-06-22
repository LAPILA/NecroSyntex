// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionComp.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/Monster/M_Spawner.h"
#include "NecroSyntex/Mission/DefenseTarget.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/Mission/MissionTrigger.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMissionComp::UMissionComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMissionComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMissionComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMissionComp::Init(class ANecroSyntexGameMode* InGameMode)
{
    CurrentGameMode = InGameMode;
    //count = 3.0f;
    ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        GS->SurvivingPlayer = GS->TotalPlayer;
    }
}

void UMissionComp::MissionSet(FName MissionName, FName RegionTag, float Duration)
{
    CurrentMissionName = MissionName;
    CurrentRegionTag = RegionTag;
    CurrentDuration = Duration;
}

void UMissionComp::CMTSet(class AMissionTrigger* MissionTrigger)
{
    CMT = MissionTrigger;
}

void UMissionComp::ActiveMission()
{
    if (CurrentMissionName == "Survival") {
        StartSurvivalMission();
    }
    else if (CurrentMissionName == "Defense") {
        StartDefenseMission();
    }
    else if (CurrentMissionName == "Rescue") {

    }
    else if (CurrentMissionName == "Boss") {

    }
    else {
        
    }
    CMT->MakeNoise();
    CMT->Destroy();
}

void UMissionComp::ActiveMonsterSpawner()
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

void UMissionComp::StopMonsterSpawner()
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

void UMissionComp::CurrentMissionFail()
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
        CurrentGameMode->LevelMissionFail();
        UE_LOG(LogTemp, Warning, TEXT("MissionName is Unavilable"));
    }
}


//Survival Mission
void UMissionComp::StartSurvivalMission()
{
    // 생존 미션 몬스터 스포너 작동
    ActiveMonsterSpawner();

    GameStateAndUIUpdate(true);

    CurrentGameMode->LevelMissionStart();

    // 생존 미션 타이머 작동
    GetWorld()->GetTimerManager().SetTimer(
        SurvivalTimerhandle,
        [this]() { SurvivalMissionSuccess(); },
        CurrentDuration,
        false
    );
}

void UMissionComp::SurvivalMissionSuccess()
{
    CurrentGameMode->LevelMissionSuccess();

    EndSurvivlvalMission();

}

void UMissionComp::SurvivalMissionFail()
{
    GetWorld()->GetTimerManager().ClearTimer(SurvivalTimerhandle);

    CurrentGameMode->LevelMissionFail();

    EndSurvivlvalMission();
}

void UMissionComp::EndSurvivlvalMission()
{
    StopMonsterSpawner();

    MissionSet("None", "None", 0.0f);

    GameStateAndUIUpdate(false);
}


//DefenseMission
void UMissionComp::StartDefenseMission()
{
    TArray<AActor*> FoundDefenseTarget;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseTarget::StaticClass(), FoundDefenseTarget);

    for (AActor* Actor : FoundDefenseTarget)
    {
        ADefenseTarget* DefenseTarget = Cast<ADefenseTarget>(Actor);
        if (DefenseTarget && DefenseTarget->RegionName == CurrentRegionTag)
        {
            DefenseTarget->DefenseObjectActive();
            CurrentDefenseTarget = DefenseTarget;
            CurrentDefenseTarget->HealthBarVisible();
        }
    }

    ActiveMonsterSpawner();

    GameStateAndUIUpdate(true);

    CurrentGameMode->LevelMissionStart();

    GetWorld()->GetTimerManager().SetTimer(
        DefenseTimerHandle,
        [this]() { DefenseMissionSuccess(); },
        CurrentDuration,
        false
    );
}

void UMissionComp::DefenseMissionSuccess()
{
    CurrentGameMode->LevelMissionSuccess();

    EndDefenseMission();
}

void UMissionComp::DefenseMissionFail()
{
    GetWorld()->GetTimerManager().ClearTimer(DefenseTimerHandle);

    EndDefenseMission();

    CurrentGameMode->LevelMissionFail();
}

void UMissionComp::EndDefenseMission()
{
    StopMonsterSpawner();

    TArray<AActor*> FoundDefenseTarget;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseTarget::StaticClass(), FoundDefenseTarget);

    for (AActor* Actor : FoundDefenseTarget)
    {
        ADefenseTarget* DefenseTarget = Cast<ADefenseTarget>(Actor);
        if (DefenseTarget && DefenseTarget->RegionName == CurrentRegionTag)
        {
            DefenseTarget->DefenseObjectDeactive();
            CurrentDefenseTarget->HealthBarHidden();
        }
    }

    MissionSet("None", "None", 0.0f);

    GameStateAndUIUpdate(false);
}

//Game State Update
void UMissionComp::GameStateAndUIUpdate(bool MissionBool)
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

void UMissionComp::MissionCountdownStart()
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

void UMissionComp::UpdateMissionCountdown()
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

void UMissionComp::MissionCountdownCancel()
{
    GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
    if (ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState())) {
        GS->MissionCountDownBool = false;
    }
}
//
//void UMissionComp::ClientStartCall_Implementation()
//{
//    MissionStart.Broadcast();
//    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ClientCall"));
//}
//
//void UMissionComp::ClientSuccessCall_Implementation()
//{
//    MissionSuccess.Broadcast();
//}
//
//void UMissionComp::ClientFailCall_Implementation()
//{
//    MissionFail.Broadcast();
//}