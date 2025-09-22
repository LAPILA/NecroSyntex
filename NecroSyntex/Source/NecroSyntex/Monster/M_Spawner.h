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

	ABasicMonsterAI* SpawnRandomMonster(UWorld* World, FVector Location, FRotator Rotation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* spawnArea;

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
	TArray<TSubclassOf<ABasicMonsterAI>>Monsters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TArray<float> MonsterSpawnRates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveIntervalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	TArray<float> spawnHealth;

	// 몬스터 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	TArray<float> spawnAttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	TArray<float> chaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delete Option")
	bool isDelete;

	UFUNCTION(BluePrintCallable)
	void StartSpawnMonster(float SpawnSpeed);

	UFUNCTION(BluePrintCallable)
	void StopSpawnMonster();

	UFUNCTION(BluePrintCallable)
	void ResetMonsterCount();

	UFUNCTION(BluePrintCallable)
	void DelayedFunction(float DelayTime);

	UFUNCTION(BluePrintCallable)
	void DeleteSpawner();

	UFUNCTION()
	void OnSpawnAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpawnAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Pahu Mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FName RegionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission", meta = (GetOptions = "GetMissionNameOptions"))
	FName MissionName;

	UFUNCTION()
	TArray<FName> GetMissionNameOptions() const
	{
		return {
			FName("Survival"),
			FName("Defense"),
			FName("Rescue"),
			FName("Boss")
		};
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	float CurrentTime;

};
