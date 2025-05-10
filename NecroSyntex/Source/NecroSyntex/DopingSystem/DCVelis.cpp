// Fill out your copyright notice in the Description page of Project Settings.


#include "DCVelis.h"

void UDCVelis::BeginPlay()
{
	Super::BeginPlay();

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

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}
}

void UDCVelis::SecondDopingUse() {

	Super::SecondDopingUse();

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}

}