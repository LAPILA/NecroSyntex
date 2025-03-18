// Fill out your copyright notice in the Description page of Project Settings.


#include "DCVelis.h"

void UDCVelis::BeginPlay()
{
	Super::BeginPlay();

	PID->MaxHealth = 150;
	PID->CurrentHealth = PID->MaxHealth;
	PID->MoveSpeed = 650.0f;
	PID->RunningSpeed = 1200.0f;
	PID->MLAtaackPoint = 50.0f;
	PID->Defense = 0;
	

}

void UDCVelis::Passive_Start()
{
	PID->RunningSpeed = PID->RunningSpeed + 100;
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
	PID->RunningSpeed = PID->RunningSpeed - 100;
	UE_LOG(LogTemp, Warning, TEXT("Velis Passive OFF"));
}