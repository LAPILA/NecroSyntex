// Fill out your copyright notice in the Description page of Project Settings.


#include "DPPainless.h"

UDPPainless::UDPPainless()
	:Super()
{

}

void UDPPainless::BuffOn(UPlayerInformData* PID)
{

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPPainless::BuffOff(UPlayerInformData* PID)
{

}

void UDPPainless::DeBuffOn(UPlayerInformData* PID)
{


	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPPainless::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPPainless::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("Painless Use"));
		//효과


		//
	}
}
