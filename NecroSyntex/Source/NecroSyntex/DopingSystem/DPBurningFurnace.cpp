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

void UDPBurningFurnace::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {
		DefenseBuffNum = PID->Defense;
		PID->Defense = PID->Defense + DefenseBuffNum;

		CheckBuff = true;

	}

	DeBuffOn(PID);
	StartBuff(PID);
}

void UDPBurningFurnace::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		PID->Defense = PID->Defense - DefenseBuffNum;

		CheckBuff = false;
	}
}

void UDPBurningFurnace::DeBuffOn(UPlayerInformData* PID)
{
	if (CheckDeBuff == false) {
		CheckDeBuff = true;

		StartDeBuff(PID);
	}
}

void UDPBurningFurnace::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {
		CheckDeBuff = false;
	}
}

void UDPBurningFurnace::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("BurningFurnace Use"));
		//효과
		BuffOn(PID);

		//
		StartCooldown();
	}
}