// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterSpawner.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::SpawnMonster()
{
	if (monsterClass == nullptr) {
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 현재 스포너 위치에 소환
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	ABasicMonsterAI* SpawnedMonster = GetWorld()->SpawnActor<ABasicMonsterAI>(monsterClass, SpawnLocation, SpawnRotation, spawnParams);

	if (SpawnedMonster) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("spawn complete."));
		float MonsterHealth = SpawnedMonster->MonsterHP;
		//FString DebugMsg = FString::Printf(TEXT("Spawn complete. Monster HP = %.2f"), MonsterHealth);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMsg);
		
		SpawnedMonster->MonsterHP = spawnHealth;
		//FString DebugMsg1 = FString::Printf(TEXT("Spawn complete. SpawnHealth = %.2f"), SpawnedMonster->MonsterHP);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMsg1);
		SpawnedMonster->MonsterAD = spawnAttackPower;
	}
}