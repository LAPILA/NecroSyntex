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

void UDPBurningFurnace::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("BurningFurnace Use"));
		//효과
		BuffOn(DopedPC);

		//
		StartCooldown();
	}
}

void UDPBurningFurnace::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		DefenseBuffNum = DopedPC->Defense;
		DopedPC->Defense = DopedPC->Defense + DefenseBuffNum;

		CheckBuff = true;

	}

	DeBuffOn(DopedPC);
	StartBuff(DopedPC);
}

void UDPBurningFurnace::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		DopedPC->Defense = DopedPC->Defense - DefenseBuffNum;

		CheckBuff = false;
	}
}

void UDPBurningFurnace::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		CheckDeBuff = true;

		StartDeBuff(DopedPC);
	}
}

void UDPBurningFurnace::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		CheckDeBuff = false;
	}
}
