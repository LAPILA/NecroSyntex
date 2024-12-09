// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{

}

void UDPSupremeStrength::BuffOn(UPlayerInformData* PID)
{
	CheckBuff = true;
	StartBuff(PID);

	DeBuffOn(PID);
}

void UDPSupremeStrength::BuffOff(UPlayerInformData* PID)
{

}

void UDPSupremeStrength::DeBuffOn(UPlayerInformData* PID)
{

	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPSupremeStrength::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPSupremeStrength::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SupremeStrength Use"));
		//효과


		//
		StartCooldown();
	}
}