// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{
	BuffDuration = 6.0f;
	DeBuffDuration = 6.0f;

	CheckBuff = false;
	CheckDeBuff = false;

}

void UDPSupremeStrength::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {
		MLAttackBuffNum = PID->MLAtaackPoint;

		PID->MLAtaackPoint = PID->MLAtaackPoint + MLAttackBuffNum;

		PID->CurrentDoped += 1;

		CheckBuff = true;
	}
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPSupremeStrength::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		PID->MLAtaackPoint = PID->MLAtaackPoint - MLAttackBuffNum;

		CheckBuff = false;
	}
}

void UDPSupremeStrength::DeBuffOn(UPlayerInformData* PID)
{
	if (CheckDeBuff == false) {
		BlurredDeBuffNum = 1;
		PID->Blurred = PID->Blurred - BlurredDeBuffNum;

		CheckDeBuff = true;
	}
	StartDeBuff(PID);
}

void UDPSupremeStrength::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {
		PID->Blurred = PID->Blurred + BlurredDeBuffNum;

		PID->CurrentDoped -= 1;

		CheckDeBuff = false;
	}
}

void UDPSupremeStrength::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SupremeStrength Use"));
		//효과


		//
		StartCooldown();
	}
}