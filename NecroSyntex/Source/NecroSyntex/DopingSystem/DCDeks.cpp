// Fill out your copyright notice in the Description page of Project Settings.


#include "DCDeks.h"

void UDCDeks::BeginPlay()
{
	Super::BeginPlay();

	PID->MaxHealth = 250;
	PID->CurrentHealth = PID->MaxHealth;
	PID->MoveSpeed = 450.0f;
	PID->RunningSpeed = 800.0f;
	PID->MLAtaackPoint = 100.0f;
	PID->Defense = 60.0f;


}

void UDCDeks::Passive_Start()
{
	PID->Defense = PID->Defense + 10.0f;
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
	PID->Defense = PID->Defense - 10.0f;
	UE_LOG(LogTemp, Warning, TEXT("Decks Passive OFF"));
}

void UDCDeks::FirstDopingUse_Implementation()
{

	Super::FirstDopingUse_Implementation();

	Passive_Start();
}

void UDCDeks::SecondDopingUse_Implementation() {

	Super::SecondDopingUse_Implementation();

	Passive_Start();

}