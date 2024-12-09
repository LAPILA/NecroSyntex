// Fill out your copyright notice in the Description page of Project Settings.


#include "DPFinalEmber.h"

UDPFinalEmber::UDPFinalEmber()
	:Super()
{

}

void UDPFinalEmber::BuffOn(UPlayerInformData* PID)
{

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPFinalEmber::BuffOff(UPlayerInformData* PID)
{

}

void UDPFinalEmber::DeBuffOn(UPlayerInformData* PID)
{


	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPFinalEmber::DeBuffOff(UPlayerInformData* PID)
{

}


void UDPFinalEmber::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("FinalEmber Use"));
		//효과



		//
	}
}
