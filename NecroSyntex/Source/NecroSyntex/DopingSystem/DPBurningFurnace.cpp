// Fill out your copyright notice in the Description page of Project Settings.


#include "DPBurningFurnace.h"

UDPBurningFurnace::UDPBurningFurnace()
	:Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;
	CheckBuff = false;
	CheckDeBuff = false;
}

bool UDPBurningFurnace::UseDopingItem(APlayerCharacter* DopedPC)
{
	UE_LOG(LogTemp, Warning, TEXT("BurningFurnace Use"));
	//효과
	if (DopedPC->Health >= (DopedPC->MaxHealth * 0.4))
	{
		BuffOn(DopedPC);
		return true;
	}
	else {
		return false;
	}
}

void UDPBurningFurnace::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {

		GetWorld()->GetTimerManager().SetTimer(
			DamageTimer,
			[this, DopedPC]() { BFDamageApply(DopedPC); },
			1.0f,
			true
		);
		CheckBuff = true;

	}

	DeBuffOn(DopedPC);
	StartBuff(DopedPC);
}

void UDPBurningFurnace::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
		CheckBuff = false;

	}
}

void UDPBurningFurnace::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {

		GetWorld()->GetTimerManager().SetTimer(
			PlayerHPMinusTimer,
			[this, DopedPC]() { PCHPMinus(DopedPC); },
			1.0f,
			true
		);

		CheckDeBuff = true;

		StartDeBuff(DopedPC);
	}
}

void UDPBurningFurnace::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		GetWorld()->GetTimerManager().ClearTimer(PlayerHPMinusTimer);

		CheckDeBuff = false;
	}
}


void UDPBurningFurnace::BFDamageApply(APlayerCharacter* DopedPC)
{
	DopedPC->CallBlueprintBurningFurnaceDamage();
}

void UDPBurningFurnace::PCHPMinus(APlayerCharacter* DopedPC)
{

	if ((DopedPC->Health - DopedPC->MaxHealth * 0.04f) <= 0.0f)
	{
		DopedPC->Health = 1.0f;
		DeBuffOff(DopedPC);
		BuffOff(DopedPC);
		TimerEnd();
	}
	else {
		DopedPC->Health -= DopedPC->MaxHealth * 0.04f;
	}

	DopedPC->UpdateHUDHealth();
}