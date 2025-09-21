// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicMonsterAI.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class NECROSYNTEX_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();

	// 소환할 몬스터의 종류 (블루프린트에서 선택 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TSubclassOf<ABasicMonsterAI> monsterClass;

	// 몬스터 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float spawnHealth = 100.0f;

	// 몬스터 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float spawnAttackPower = 10.0f;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats")
	TArray<float> monsterSpawnRates;*/

	// 실제 스폰 함수
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnMonster();

};
