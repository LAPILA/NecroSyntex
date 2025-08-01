// Fill out your copyright notice in the Description page of Project Settings.


#include "M_Spawner.h"
#include "Engine.h"
#include "BasicMonsterAI.h"
#include "EliteMonsterAI.h"
#include "TenAxe_MonsterAI.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"

// Sets default values
AM_Spawner::AM_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isSpawn = false;

	Spawner = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnPoint"));
	RootComponent = Spawner;

	CurrentMonsterCount = 0;
	MaxMonster = 50;
	MonsterSpawnSpeed = 5.0f;
	WaveIntervalTime = 5.0f;

	isSpawn = false;
	isWave = false;

	RegionTag = "Survival";
}

// Called when the game starts or when spawned
void AM_Spawner::BeginPlay()
{
	Super::BeginPlay();

	//StartSpawnMonster(MonsterSpawnSpeed);
}

// Called every frame
void AM_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentTime = CurrentTime + 1 * DeltaTime;

	if (!isSpawn) {
		return;
	}

	if (MaxMonster == CurrentMonsterCount) {
		StopSpawnMonster();
		if (isWave) { //if Wave is true. Wait WaveIntervalTime and Monster count reset. 
			StartSpawnMonster(MonsterSpawnSpeed);
			DelayedFunction(WaveIntervalTime);
			ResetMonsterCount();
		}
	}

	if (CurrentTime >= MonsterSpawnSpeed && isSpawn && MaxMonster > CurrentMonsterCount) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = GetInstigator();

		UWorld* WRLD = GetWorld();

		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		//ABasicMonsterAI* PlayMonster = WRLD->SpawnActor<ABasicMonsterAI>(MyMonster[0], Location, Rotation);
		ABasicMonsterAI* PlayMonster = SpawnRandomMonster(WRLD, Location, Rotation);

		if (PlayMonster) {
			CurrentMonsterCount++;
		}

		CurrentTime = 0.0f;
	}
}

ABasicMonsterAI* AM_Spawner::SpawnRandomMonster(UWorld* World, FVector Location, FRotator Rotation)
{
	if (MyMonsters.Num() != MonsterSpawnRates.Num()) {
		return nullptr;  // 몬스터 종류와 비율 배열의 길이가 맞지 않으면 nullptr 반환
	}

	// 총 확률의 합을 구합니다
	float TotalWeight = 0.0f;
	for (float Rate : MonsterSpawnRates) {
		TotalWeight += Rate;
	}

	// 랜덤 값 생성 (0.0 ~ TotalWeight 사이의 값)
	float RandomValue = FMath::RandRange(0.0f, TotalWeight);

	// 몬스터를 랜덤 확률에 따라 선택
	float CumulativeWeight = 0.0f;
	for (int32 i = 0; i < MyMonsters.Num(); i++) {
		CumulativeWeight += MonsterSpawnRates[i];

		if (RandomValue <= CumulativeWeight) {
			// 해당 몬스터를 생성합니다
			return World->SpawnActor<ABasicMonsterAI>(MyMonsters[i], Location, Rotation);
		}
	}

	return nullptr;  // 예외 처리
}

void AM_Spawner::StartSpawnMonster(float SpawnSpeed)
{
	isSpawn = true;
	MonsterSpawnSpeed = SpawnSpeed;
}

void AM_Spawner::StopSpawnMonster()
{
	isSpawn = false;
}

void AM_Spawner::DelayedFunction(float DelayTime)
{
	WaveIntervalTime = DelayTime;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AM_Spawner::ResetMonsterCount, WaveIntervalTime, false);
}

void AM_Spawner::ResetMonsterCount()
{
	CurrentMonsterCount = 0;
}