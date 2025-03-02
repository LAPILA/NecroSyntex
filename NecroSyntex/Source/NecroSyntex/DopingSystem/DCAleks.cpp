// Fill out your copyright notice in the Description page of Project Settings.


#include "DCAleks.h"

void UDCAleks::BeginPlay()
{
	Super::BeginPlay();

	PID->MaxHealth = 200;
	PID->CurrentHealth = PID->MaxHealth;
	PID->MaxShield = 75.0f;
	PID->CurrentShield = PID->CurrentShield;
	PID->MoveSpeed = 550.0f;
	PID->RunningSpeed = 1000.0f;
	PID->MLAtaackPoint = 200.0f;
	PID->Defense = 30;


}

void UDCAleks::Passive_Start()
{	

	UE_LOG(LogTemp, Warning, TEXT("Aleks Passive On"));

	PID->MaxHealth += 20.0f;

	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this]() {Passive_End(); },
		Passive_Duration,
		false
	);
}

void UDCAleks::Passive_End()
{

	UE_LOG(LogTemp, Warning, TEXT("Aleks Passive OFF"));

	PID->MaxHealth -= 20.0f;
}


void UDCAleks::FirstDopingUse_Implementation() {

	Super::FirstDopingUse_Implementation();

	Passive_Start();
}

void UDCAleks::SecondDopingUse_Implementation() {

	Super::SecondDopingUse_Implementation();

	Passive_Start();

}