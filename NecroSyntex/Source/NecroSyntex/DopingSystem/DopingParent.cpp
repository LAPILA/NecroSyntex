// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingParent.h"

UDopingParent::UDopingParent()
{
	DopingItemNum = 5;
	DopingCoolTime = 10.0f;
	CurrentCoolTime = 0.0f;
	BuffDuration = 10.0f;
	BuffRemainDuration = 0.0f;
	DeBuffDuration = 10.0f;
	DeBuffRemainDuration = 0.0f;
	CheckBuff = false;
	CheckDeBuff = false;
	Able = true;
}

void UDopingParent::UseDopingItem(UPlayerInformData* PID)
{

}

void UDopingParent::StartCooldown()
{

}

void UDopingParent::BuffOn(UPlayerInformData* PID)
{

}

void UDopingParent::BuffOff(UPlayerInformData* PID)
{

}

void UDopingParent::DeBuffOn(UPlayerInformData* PID)
{

}

void UDopingParent::DeBuffOff(UPlayerInformData* PID)
{

}

/*
void UDopingParent::GetDeltaTime(float DeltaTime)
{
	XDeltaTime = DeltaTime;
}
*/