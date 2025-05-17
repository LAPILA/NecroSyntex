// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseTarget.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADefenseTarget::ADefenseTarget()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ADefenseTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADefenseTarget, Health);
	DOREPLIFETIME(ADefenseTarget, Survive);
	DOREPLIFETIME(ADefenseTarget, Active);
}

// Called when the game starts or when spawned
void ADefenseTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefenseTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefenseTarget::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADefenseTarget::DefenseObjectActive_Implementation()
{
	Survive = true;
	Active = true;
}

void ADefenseTarget::DefenseObjectDestroy_Implementation()
{
	ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();

	Survive = false;
	Active = false;

	NecroSyntexGameMode->MissionManager->DefenseMissionFail();
}

void ADefenseTarget::DefenseObjectDeactive_Implementation()
{
	Active = false;
}

void ADefenseTarget::TakedDamage_Implementation(AActor* DamagedActor, float Damage)
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
	HealthBarUpdate();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damaged"));

	if (Health <= 0)
	{
		DefenseObjectDestroy();
		Destroy();
	}
}