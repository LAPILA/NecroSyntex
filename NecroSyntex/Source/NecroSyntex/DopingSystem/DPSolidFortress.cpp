// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSolidFortress.h"

UDPSolidFortress::UDPSolidFortress()
	:Super()
{

}

void UDPSolidFortress::BuffOn(UPlayerInformData* PID)
{

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPSolidFortress::BuffOff(UPlayerInformData* PID)
{

}

void UDPSolidFortress::DeBuffOn(UPlayerInformData* PID)
{


	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPSolidFortress::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPSolidFortress::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SolidFortress Use"));
		//효과



		//
	}
}
