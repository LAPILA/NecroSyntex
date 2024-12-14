// Fill out your copyright notice in the Description page of Project Settings.


#include "DPFinalEmber.h"

UDPFinalEmber::UDPFinalEmber()
	:Super()
{

}

void UDPFinalEmber::BuffOn(UPlayerInformData* PID)
{


	CheckBuff = true;
	StartBuff(PID);
}

void UDPFinalEmber::BuffOff(UPlayerInformData* PID)
{
	DeBuffOn(PID);
}

void UDPFinalEmber::DeBuffOn(UPlayerInformData* PID)
{


	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPFinalEmber::DeBuffOff(UPlayerInformData* PID)
{

}


void UDPFinalEmber::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("FinalEmber Use"));
		//효과
		BuffOn(PID);
		//
		StartCooldown();
	}
}
