// Fill out your copyright notice in the Description page of Project Settings.


#include "M_Spawner.h"
#include "Engine.h"
#include "BasicMonsterAI.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
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
	MaxMonster = 10;
	MonsterSpawnSpeed = 5.0f;
}

// Called when the game starts or when spawned
void AM_Spawner::BeginPlay()
{
	Super::BeginPlay();

	StartSpawnMonster(MonsterSpawnSpeed);
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
		//MonsterAI->OnMonsterDestroyed.AddDynamic(this, &AM_S)
	}

	if (CurrentTime >= MonsterSpawnSpeed && isSpawn && MaxMonster > CurrentMonsterCount) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = GetInstigator();

		UWorld* WRLD = GetWorld();

		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		ABasicMonsterAI* PlayMonster = WRLD->SpawnActor<ABasicMonsterAI>(MyMonster[0], Location, Rotation);

		if (PlayMonster) {
			/*UE_LOG(LogTemp, Warning, TEXT("if(PlayMonster)"));
			PlayMonster->OnMonsterDestroyed.AddDynamic(this, &AM_Spawner::ReduceMonster);*/
			//PlayMonster->OnMonsterDestroyed.AddDynamic(this, &AM_Spawner::OnMonsterDestroyed);
			CurrentMonsterCount++;
		}
		UE_LOG(LogTemp, Warning, TEXT("00"));
		CurrentTime = 0.0f;
	}
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

void AM_Spawner::ReduceMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TEXT("OnMonsterDestroyed")));
	UE_LOG(LogTemp, Warning, TEXT("OnMonsterDestroyed"));
	CurrentMonsterCount--;
}