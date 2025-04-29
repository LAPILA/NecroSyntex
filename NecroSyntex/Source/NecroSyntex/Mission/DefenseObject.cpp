// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseObject.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADefenseObject::ADefenseObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
	Survive = false;
	Active = false;
}

void ADefenseObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADefenseObject, Health);
	DOREPLIFETIME(ADefenseObject, Survive);
	DOREPLIFETIME(ADefenseObject, Active);
}

// Called when the game starts or when spawned
void ADefenseObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefenseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefenseObject::DefenseObjectActive_Implementation()
{
	Survive = true;
	Active = true;
}

void ADefenseObject::DefenseObjectDestroy_Implementation()
{
	ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();

	NecroSyntexGameMode->MissionManager->DefenseMissionFail();
}

void ADefenseObject::TakedDamage_Implementation(AActor* DamagedActor, float Damage)
{
	if (!Active || !Survive)
	{
		return;
	}

	if (Health <= 0.0f)
	{
		return;
	}

	Health -= Damage;

	if (Health <= 0)
	{
		DefenseObjectDestroy();
		Destroy();
	}
}