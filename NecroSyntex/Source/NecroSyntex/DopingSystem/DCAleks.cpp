// Fill out your copyright notice in the Description page of Project Settings.


#include "DCAleks.h"

void UDCAleks::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("1"));

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;


	if (OwnerCharacter->HasAuthority())
	{
		OwnerCharacter->MaxHealth = 200.0f;
		OwnerCharacter->Health = 200.0f;
		OwnerCharacter->MaxShield = 75.0f;
		OwnerCharacter->Shield = 75.0f;
		OwnerCharacter->WalkSpeed = 550.0f;
		OwnerCharacter->RunningSpeed = 1000.0f;
		OwnerCharacter->MLAtaackPoint = 200.0f;
		OwnerCharacter->Defense = 30;
	}


}

void UDCAleks::Passive_Start()
{	

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;


	if (OwnerCharacter->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("Aleks Passive On"));

		OwnerCharacter->MaxHealth += 20.0f;

		GetWorld()->GetTimerManager().SetTimer(
			PassiveTimerHandle,
			[this]() {Passive_End(); },
			Passive_Duration,
			false
		);
	}
}

void UDCAleks::Passive_End()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("Aleks Passive OFF"));

		OwnerCharacter->MaxHealth -= 20.0f;
	}
}


void UDCAleks::FirstDopingUse() {

	Super::FirstDopingUse();

	Passive_Start();
}

void UDCAleks::SecondDopingUse() {

	Super::SecondDopingUse();

	Passive_Start();

}