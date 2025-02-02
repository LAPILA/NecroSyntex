// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{
	BuffDuration = 6.0f;
	DeBuffDuration = 6.0f;

}

void UDPSupremeStrength::BuffOn(UPlayerInformData* PID)
{
	MLAttackBuffNum = PID->MLAtaackPoint;

	PID->MLAtaackPoint = PID->MLAtaackPoint + MLAttackBuffNum;

	PID->CurrentDoped += 1;

	CheckBuff = true;
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPSupremeStrength::BuffOff(UPlayerInformData* PID)
{
	PID->MLAtaackPoint = PID->MLAtaackPoint - MLAttackBuffNum;

	CheckBuff = false;
}

void UDPSupremeStrength::DeBuffOn(UPlayerInformData* PID)
{
	BlurredDeBuffNum = 1;
	PID->Blurred = PID->Blurred - BlurredDeBuffNum;

	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPSupremeStrength::DeBuffOff(UPlayerInformData* PID)
{
	PID->Blurred = PID->Blurred + BlurredDeBuffNum;

	PID->CurrentDoped -= 1;

	CheckDeBuff = false;
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