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

void UDPPainless::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		WalkingBuffNum = DopedPC->WalkSpeed * 0.1f;
		RunningBuffNum = DopedPC->RunningSpeed * 0.1f;


		DopedPC->DopingDamageBuff += 10.0f;


		DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningBuffNum;

		DopedPC->CurrentDoped += 1;

		CheckBuff = true;
	}

	StartBuff(DopedPC);

	DeBuffOn(DopedPC);
}

void UDPPainless::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		DopedPC->DopingDamageBuff -= 10.0f;


		DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningBuffNum;
		CheckBuff = false;
	}
}

void UDPPainless::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		//체력 회복량 50% 감소?

		CheckDeBuff = true;
	}


	StartDeBuff(DopedPC);
}

void UDPPainless::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		DopedPC->CurrentDoped -= 1;
		CheckDeBuff = false;
	}
}

void UDPPainless::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("Painless Use"));
		//효과
		BuffOn(DopedPC);
		//
		StartCooldown();
	}
}
