// Fill out your copyright notice in the Description page of Project Settings.


#include "DCElis.h"

void UDCElis::BeginPlay()
{
	Super::BeginPlay();

	PID->MaxHealth = 200;
	PID->CurrentHealth = PID->MaxHealth;
	PID->MoveSpeed = 550.0f;
	PID->RunningSpeed = 1000.0f;
	PID->MLAtaackPoint = 60.0f;
	PID->Defense = 20;


}

void UDCElis::Passive_Start()
{
	UE_LOG(LogTemp, Warning, TEXT("Velis Passive On"));
	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this]() {Passive_End(); },
		Passive_Duration,
		false
	);
}

void UDCElis::Passive_End()
{
	UE_LOG(LogTemp, Warning, TEXT("Velis Passive OFF"));
}