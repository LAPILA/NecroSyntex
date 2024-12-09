// Fill out your copyright notice in the Description page of Project Settings.


#include "DPPainless.h"

UDPPainless::UDPPainless()
	:Super()
{

}

void UDPPainless::BuffOn(UPlayerInformData* PID)
{

	CheckBuff = true;
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPPainless::BuffOff(UPlayerInformData* PID)
{

}

void UDPPainless::DeBuffOn(UPlayerInformData* PID)
{


	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPPainless::DeBuffOff(UPlayerInformData* PID)
{

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
