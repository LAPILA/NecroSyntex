// Fill out your copyright notice in the Description page of Project Settings.


#include "DPHallucinationShield.h"

UDPHallucinationShield::UDPHallucinationShield()
	:Super()
{
	DeBuffDuration = 1.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPHallucinationShield::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("HallucinationShield Use"));
		//È¿°ú
		BuffOn(DopedPC);

		//
		StartCooldown();
	}
}

void UDPHallucinationShield::BuffOn(APlayerCharacter* DopedPC)
{

	if (DopedPC->Shield + 50.0f >= DopedPC->MaxShield) {
		DopedPC->Shield = DopedPC->MaxShield;
	}
	else {
		DopedPC->Shield += 50.0f;
	}

	DeBuffOn(DopedPC);

	StartBuff(DopedPC);
}

void UDPHallucinationShield::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {

		
	}

}

void UDPHallucinationShield::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {

		CallCameraShake();

		DopedPC->HSDeBuffON();

		CheckDeBuff = true;
	}

	StartDeBuff(DopedPC);
}

void UDPHallucinationShield::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {



		CheckDeBuff = false;
	}
}

void UDPHallucinationShield::CallCameraShake()
{

	return;
}