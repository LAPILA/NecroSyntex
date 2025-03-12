// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterSpawner.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::StartSpawning()
{
    CurrentMonsterCount = 0;
    GetWorld()->GetTimerManager().SetTimer(SpawnWaveTimer, this, &AMonsterSpawner::SpawnMonster, MonsterSpawnRate, true);
}

void AMonsterSpawner::StopSpawning()
{
    GetWorld()->GetTimerManager().ClearTimer(SpawnWaveTimer);
    GetWorld()->GetTimerManager().ClearTimer(MonsterSpawnTimer);
}

void AMonsterSpawner::SpawnMonster()
{
    if (CurrentMonsterCount < MaxMonstersPerSpawn)
    {
        FVector2D RandPoint = FMath::RandPointInCircle(SpawnRadius);
        FVector Location = GetActorLocation() + FVector(RandPoint.X, RandPoint.Y, 0);
        GetWorld()->SpawnActor<AActor>(MonsterClass, Location, FRotator::ZeroRotator);

        CurrentMonsterCount++;
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(MonsterSpawnTimer);
        GetWorld()->GetTimerManager().SetTimer(SpawnWaveTimer, this, &AMonsterSpawner::StartSpawning, SpawnInterval, false);
        CurrentMonsterCount = 0;
    }
}