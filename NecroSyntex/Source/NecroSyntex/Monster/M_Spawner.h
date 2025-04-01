// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicMonsterAI.h"
#include "M_Spawner.generated.h"

UCLASS()
class NECROSYNTEX_API AM_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AM_Spawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxMonster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentMonsterCount;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterSpawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isWave;

	UPROPERTY(EditAnywhere, Category = "Monster")
	class UBoxComponent* Spawner;

	UPROPERTY(EditAnywhere, Category = "Monster")
	TSubclassOf<AActor>ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Monster")
	TArray<TSubclassOf<AActor>>MyMonster;

	UPROPERTY(EditAnywhere, Category = "Monster")
	TArray<ABasicMonsterAI*> SpawnMonster;

	UPROPERTY(EditAnywhere, Category = "Monster")
	float WaveIntervalTime;

	UPROPERTY(EditAnywhere, Category = "Monster")
	bool isSpawn;

	UFUNCTION(BluePrintCallable)
	void StartSpawnMonster(float SpawnSpeed);

	void StopSpawnMonster();

	void ResetMonsterCount();

	void DelayedFunction(float DelayTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	float CurrentTime;

};
