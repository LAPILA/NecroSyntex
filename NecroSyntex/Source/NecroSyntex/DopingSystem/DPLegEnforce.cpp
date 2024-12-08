// Fill out your copyright notice in the Description page of Project Settings.


#include "DPLegEnforce.h"

UDPLegEnforce::UDPLegEnforce()
	: Super()
{
	BuffDuration = 5.0f;
	DeBuffDuration = 2.0f;

	WalkingBuffNum = 1200.0f;
	RunningBuffNum = 1600.0f;
	WalkingDeBuffNum = 50.0f;
	RunningDeBuffNum = 100.0f;

}

void UDPLegEnforce::BuffOn(UPlayerInformData* PID)
{
	PID->MoveSpeed = PID->MoveSpeed + WalkingBuffNum;
	PID->RunningSpeed = PID->RunningSpeed + RunningBuffNum;

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPLegEnforce::BuffOff(UPlayerInformData* PID)
{
	PID->MoveSpeed = PID->MoveSpeed - WalkingBuffNum;
	PID->RunningSpeed = PID->RunningSpeed - RunningBuffNum;

	DeBuffOn(PID);
}

void UDPLegEnforce::DeBuffOn(UPlayerInformData* PID)
{
	PID->MoveSpeed = PID->MoveSpeed - WalkingDeBuffNum;
	PID->RunningSpeed = PID->RunningSpeed - RunningDeBuffNum;

	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPLegEnforce::DeBuffOff(UPlayerInformData* PID)
{
	PID->MoveSpeed = PID->MoveSpeed + WalkingDeBuffNum;
	PID->RunningSpeed = PID->RunningSpeed + RunningDeBuffNum;
}

void UDPLegEnforce::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("LegEnforce Use"));
		//효과
		BuffOn(PID);


		//
	}
}