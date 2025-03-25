// Fill out your copyright notice in the Description page of Project Settings.


#include "DPHPconversion.h"

UDPHPconversion::UDPHPconversion()
	: Super()
{
	BuffDuration = 5.0f;
	DeBuffDuration = 2.0f;


	CheckBuff = false;
	CheckDeBuff = false;

}

void UDPHPconversion::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;

		// Buff 적용 및 타이머 시작
		BuffOn(PID);

		// 쿨타임 시작
		StartCooldown();

		UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion Use"));
	}
}

void UDPHPconversion::BuffOn(UPlayerInformData* PID)
{
	if (PID->CurrentShield + 50.0f > PID->MaxShield)
	{
		PID->CurrentShield = PID->MaxShield;
	}
	else {
		PID->CurrentShield += 50.0f;
	}
	DeBuffOn(PID);

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion BuffOn"));
}

void UDPHPconversion::BuffOff(UPlayerInformData* PID)
{

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion BuffOff"));
}

void UDPHPconversion::DeBuffOn(UPlayerInformData* PID)
{

	if (PID->CurrentHealth - 20.0f <= 0) {
		PID->CurrentHealth = 1.0f;
	}
	else {
		PID->CurrentHealth -= 20.0f;
	}


	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion DeBuffOn."));
}

void UDPHPconversion::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {


		PID->CurrentDoped -= 1;

		CheckDeBuff = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion DeBuffOff: Speed restored."));
}
