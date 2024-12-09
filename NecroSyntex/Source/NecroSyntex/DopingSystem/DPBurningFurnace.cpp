// Fill out your copyright notice in the Description page of Project Settings.


#include "DPBurningFurnace.h"

UDPBurningFurnace::UDPBurningFurnace()
	:Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;
}

void UDPBurningFurnace::BuffOn(UPlayerInformData* PID)
{
	DefenseBuffNum = PID->Defense;
	PID->Defense = PID->Defense + DefenseBuffNum;

	CheckBuff = true;

	StartBuff(PID);
	DeBuffOn(PID);
}

void UDPBurningFurnace::BuffOff(UPlayerInformData* PID)
{
	PID->Defense = PID->Defense - DefenseBuffNum;

	CheckBuff = false;
}

void UDPBurningFurnace::DeBuffOn(UPlayerInformData* PID)
{

	CheckDeBuff = true;

	StartDeBuff(PID);
}

void UDPBurningFurnace::DeBuffOff(UPlayerInformData* PID)
{


	CheckDeBuff = false;
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