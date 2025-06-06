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

void UDPHPconversion::UseDopingItem(APlayerCharacter* DopedPC)
{
	// Buff 적용 및 타이머 시작
	BuffOn(DopedPC);
}

void UDPHPconversion::BuffOn(APlayerCharacter* DopedPC)
{
	if (DopedPC->Shield + 50.0f > DopedPC->MaxShield)
	{
		DopedPC->Shield = DopedPC->MaxShield;
	}
	else {
		DopedPC->Shield += 50.0f;
	}
	DeBuffOn(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion BuffOn"));
}

void UDPHPconversion::BuffOff(APlayerCharacter* DopedPC)
{

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion BuffOff"));
}

void UDPHPconversion::DeBuffOn(APlayerCharacter* DopedPC)
{

	if (DopedPC->Health - 20.0f <= 0) {
		DopedPC->Health = 1.0f;
	}
	else {
		DopedPC->Health -= 20.0f;
	}


	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion DeBuffOn."));
}

void UDPHPconversion::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {


		DopedPC->CurrentDoped -= 1;

		CheckDeBuff = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDPHPconversion DeBuffOff: Speed restored."));
}
