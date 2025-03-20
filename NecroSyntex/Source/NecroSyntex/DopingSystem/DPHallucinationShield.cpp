// Fill out your copyright notice in the Description page of Project Settings.


#include "DPHallucinationShield.h"

UDPHallucinationShield::UDPHallucinationShield()
	:Super()
{
	DeBuffDuration = 1.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPHallucinationShield::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("HallucinationShield Use"));
		//È¿°ú
		BuffOn(PID);

		//
		StartCooldown();
	}
}

void UDPHallucinationShield::BuffOn(UPlayerInformData* PID)
{

	if (PID->CurrentShield + 50.0f >= PID->MaxShield) {
		PID->CurrentShield = PID->MaxShield;
	}
	else {
		PID->CurrentShield += 50.0f;
	}

	DeBuffOn(PID);

	StartBuff(PID);
}

void UDPHallucinationShield::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {

		
	}

}

void UDPHallucinationShield::DeBuffOn(UPlayerInformData* PID)
{
	if (CheckDeBuff == false) {

		CallCameraShake();

		CheckDeBuff = true;
	}

	StartDeBuff(PID);
}

void UDPHallucinationShield::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {



		CheckDeBuff = false;
	}
}

void UDPHallucinationShield::CallCameraShake()
{

	return;
}