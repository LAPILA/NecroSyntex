// Fill out your copyright notice in the Description page of Project Settings.


#include "DCDeks.h"

void UDCDeks::BeginPlay()
{
	Super::BeginPlay();


	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->HasAuthority())
	{
		OwnerCharacter->MaxHealth = 250;
		OwnerCharacter->Health = OwnerCharacter->MaxHealth;
		OwnerCharacter->WalkSpeed = 450.0f;
		OwnerCharacter->RunningSpeed = 800.0f;
		OwnerCharacter->MLAtaackPoint = 100.0f;
		OwnerCharacter->Defense = 60.0f;
	}


}

void UDCDeks::Passive_Start()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerCharacter->Defense = OwnerCharacter->Defense + 10.0f;
	UE_LOG(LogTemp, Warning, TEXT("Decks Passive On"));
	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this]() {Passive_End(); },
		Passive_Duration,
		false
	);
}

void UDCDeks::Passive_End()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerCharacter->Defense = OwnerCharacter->Defense - 10.0f;
	UE_LOG(LogTemp, Warning, TEXT("Decks Passive OFF"));
}

void UDCDeks::FirstDopingUse()
{

	Super::FirstDopingUse();

	Passive_Start();
}

void UDCDeks::SecondDopingUse() {

	Super::SecondDopingUse();

	Passive_Start();

}