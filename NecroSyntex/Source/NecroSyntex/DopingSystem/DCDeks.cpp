// Fill out your copyright notice in the Description page of Project Settings.


#include "DCDeks.h"

void UDCDeks::BeginPlay()
{
	Super::BeginPlay();

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
	passive_call = true;
}

void UDCDeks::FirstDopingUse()
{

	Super::FirstDopingUse();

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}
}

void UDCDeks::SecondDopingUse() {

	Super::SecondDopingUse();

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}

}