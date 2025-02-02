// Fill out your copyright notice in the Description page of Project Settings.


#include "DPPainless.h"

UDPPainless::UDPPainless()
	:Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPPainless::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {
		WalkingBuffNum = PID->MoveSpeed * 0.1f;
		RunningBuffNum = PID->RunningSpeed * 0.1f;


		PID->DopingDamageBuff += 10.0f;


		PID->MoveSpeed = PID->MoveSpeed + WalkingBuffNum;
		PID->RunningSpeed = PID->RunningSpeed + RunningBuffNum;

		PID->CurrentDoped += 1;

		CheckBuff = true;
	}

	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPPainless::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		PID->DopingDamageBuff -= 10.0f;


		PID->MoveSpeed = PID->MoveSpeed - WalkingBuffNum;
		PID->RunningSpeed = PID->RunningSpeed - RunningBuffNum;
		CheckBuff = false;
	}
}

void UDPPainless::DeBuffOn(UPlayerInformData* PID)
{
	if (CheckDeBuff == false) {
		//체력 회복량 50% 감소?

		CheckDeBuff = true;
	}


	StartDeBuff(PID);
}

void UDPPainless::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {
		PID->CurrentDoped -= 1;
		CheckDeBuff = false;
	}
}

void UDPPainless::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("Painless Use"));
		//효과
		BuffOn(PID);
		//
		StartCooldown();
	}
}
