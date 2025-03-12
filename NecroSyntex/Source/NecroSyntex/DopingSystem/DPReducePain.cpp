// Fill out your copyright notice in the Description page of Project Settings.


#include "DPReducePain.h"

UDPReducePain::UDPReducePain()
	:Super()
{
	BuffDuration = 2.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPReducePain::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {
		DefenseBuffNum = 50.0f - PID->Defense;
		PID->Defense = PID->Defense + DefenseBuffNum;

		PID->CurrentDoped += 1;

		CheckBuff = true;
		UE_LOG(LogTemp, Warning, TEXT("ReducePain Use"));
	}

	DeBuffOn(PID);
	StartBuff(PID);
}

void UDPReducePain::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		PID->Defense = PID->Defense - DefenseBuffNum;
		CheckBuff = false;
	}
	
}

void UDPReducePain::DeBuffOn(UPlayerInformData* PID)
{
	if (CheckDeBuff == false) {
		PID->CurrentHealth = PID->CurrentHealth - (PID->CurrentHealth * 0.3);
		PID->CurrentDoped -= 1;
	}
}

void UDPReducePain::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPReducePain::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("ReducePain Use"));
		//효과
		BuffOn(PID);

		//
		StartCooldown();
	}
}