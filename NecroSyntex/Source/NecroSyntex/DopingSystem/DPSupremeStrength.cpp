// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{

}

void UDPSupremeStrength::BuffOn(UPlayerInformData* PID)
{

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPSupremeStrength::BuffOff(UPlayerInformData* PID)
{

}

void UDPSupremeStrength::DeBuffOn(UPlayerInformData* PID)
{


	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPSupremeStrength::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPSupremeStrength::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SupremeStrength Use"));
		//효과


		//
	}
}