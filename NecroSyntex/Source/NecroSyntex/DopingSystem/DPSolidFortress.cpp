// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSolidFortress.h"

UDPSolidFortress::UDPSolidFortress()
	:Super()
{

}

void UDPSolidFortress::BuffOn(UPlayerInformData* PID)
{

	CheckBuff = true;
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPSolidFortress::BuffOff(UPlayerInformData* PID)
{

}

void UDPSolidFortress::DeBuffOn(UPlayerInformData* PID)
{

	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPSolidFortress::DeBuffOff(UPlayerInformData* PID)
{

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
