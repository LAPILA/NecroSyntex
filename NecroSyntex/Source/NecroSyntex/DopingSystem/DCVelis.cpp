// Fill out your copyright notice in the Description page of Project Settings.


#include "DCVelis.h"

void UDCVelis::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->HasAuthority())
	{
		OwnerCharacter->MaxHealth = 150;
		OwnerCharacter->Health = OwnerCharacter->MaxHealth;
		OwnerCharacter->WalkSpeed = 650.0f;
		OwnerCharacter->RunningSpeed = 1200.0f;
		OwnerCharacter->MLAtaackPoint = 50.0f;
		OwnerCharacter->Defense = 0;
	}
	

}

void UDCVelis::Passive_Start()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerCharacter->RunningSpeed = OwnerCharacter->RunningSpeed + 100;
	UE_LOG(LogTemp, Warning, TEXT("Velis Passive On"));
	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this]() {Passive_End(); },
		Passive_Duration,
		false
	);
}

void UDCVelis::Passive_End()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerCharacter->RunningSpeed = OwnerCharacter->RunningSpeed - 100;
	UE_LOG(LogTemp, Warning, TEXT("Velis Passive OFF"));
}

void UDCVelis::FirstDopingUse()
{

	Super::FirstDopingUse();

	Passive_Start();
}

void UDCVelis::SecondDopingUse() {

	Super::SecondDopingUse();

	Passive_Start();

}