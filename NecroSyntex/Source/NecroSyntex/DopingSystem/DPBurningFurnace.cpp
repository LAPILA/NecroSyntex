// Fill out your copyright notice in the Description page of Project Settings.


#include "DPBurningFurnace.h"

UDPBurningFurnace::UDPBurningFurnace()
	:Super()
{

}

void UDPBurningFurnace::BuffOn(UPlayerInformData* PID)
{

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPBurningFurnace::BuffOff(UPlayerInformData* PID)
{

}

void UDPBurningFurnace::DeBuffOn(UPlayerInformData* PID)
{


	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPBurningFurnace::DeBuffOff(UPlayerInformData* PID)
{

}

void UDPBurningFurnace::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("BurningFurnace Use"));
		//효과
		PID->Defense = PID->Defense * 2;

		//
	}
}