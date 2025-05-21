// Fill out your copyright notice in the Description page of Project Settings.


#include "DCAleks.h"

void UDCAleks::BeginPlay()
{
	Super::BeginPlay();

}

void UDCAleks::Passive_Start()
{	

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;



	if (OwnerCharacter->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("Aleks Passive On"));

		OwnerCharacter->MaxHealth += 20.0f;
		OwnerCharacter->UpdateHUDHealth();

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
		OwnerCharacter->UpdateHUDHealth();
		passive_call = true;
	}
}


void UDCAleks::FirstDopingUse() {

	Super::FirstDopingUse();

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}
}

void UDCAleks::SecondDopingUse() {

	Super::SecondDopingUse();

	if (passive_call) {
		Passive_Start();
		passive_call = false;
	}

}