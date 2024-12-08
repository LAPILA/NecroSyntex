// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class NECROSYNTEX_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StartSpawning();

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StopSpawning();

    UFUNCTION()
    void SpawnMonster();

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int MaxMonstersPerSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float MonsterSpawnRate;

private:
    FTimerHandle SpawnWaveTimer;
    FTimerHandle MonsterSpawnTimer;
    int CurrentMonsterCount;

};
