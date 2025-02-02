// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSolidFortress.h"

UDPSolidFortress::UDPSolidFortress()
	:Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;
}

void UDPSolidFortress::BuffOn(UPlayerInformData* PID)
{
	HPBuffNum = PID->MaxHealth * 0.5f;

	PID->MaxHealth = PID->MaxHealth + HPBuffNum;

	PID->CurrentDoped += 1;


	CheckBuff = true;
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPSolidFortress::BuffOff(UPlayerInformData* PID)
{
	PID->MaxHealth = PID->MaxHealth - HPBuffNum;
	CheckBuff = false;
}

void UDPSolidFortress::DeBuffOn(UPlayerInformData* PID)
{
	WalkingDeBuffNum = PID->MoveSpeed * 0.2f;
	RunningDeBuffNum = PID->RunningSpeed * 0.2f;

	PID->MoveSpeed = PID->MoveSpeed - WalkingDeBuffNum;
	PID->RunningSpeed = PID->RunningSpeed - RunningDeBuffNum;
	PID->DopingDamageBuff -= 20.0f;

	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPSolidFortress::DeBuffOff(UPlayerInformData* PID)
{
	PID->MoveSpeed = PID->MoveSpeed + WalkingDeBuffNum;
	PID->RunningSpeed = PID->RunningSpeed + RunningDeBuffNum;
	PID->DopingDamageBuff += 20.0f;

	PID->CurrentDoped -= 1;
	CheckDeBuff = false;
}

void UDPSolidFortress::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SolidFortress Use"));
		//효과
		BuffOn(PID);
		//
		StartCooldown();
	}
}
