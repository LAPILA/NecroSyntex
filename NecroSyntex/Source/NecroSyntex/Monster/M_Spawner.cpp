// Fill out your copyright notice in the Description page of Project Settings.


#include "M_Spawner.h"
#include "Components/BoxComponent.h"

// Sets default values
AM_Spawner::AM_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnPoint"));
}

// Called when the game starts or when spawned
void AM_Spawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AM_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

