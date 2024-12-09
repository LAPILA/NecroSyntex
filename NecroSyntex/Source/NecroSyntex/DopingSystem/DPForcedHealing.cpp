// Fill out your copyright notice in the Description page of Project Settings.


#include "DPForcedHealing.h"

UDPForcedHealing::UDPForcedHealing() : Super()
{
	BuffDuration = 2.0f;
	DeBuffDuration = 6.0f;
}

void UDPForcedHealing::BuffOn(UPlayerInformData* PID)
{
	targetRecover = (PID->MaxHealth * 0.3);
	BuffRecoverAPS = targetRecover / BuffDuration;
	PID->RecoverAPS = PID->RecoverAPS + BuffRecoverAPS;

	BuffRemainDuration = BuffDuration;
	CheckBuff = true;
}

void UDPForcedHealing::BuffOff(UPlayerInformData* PID)
{
	PID->RecoverAPS = PID->RecoverAPS - BuffRecoverAPS;
}

void UDPForcedHealing::DeBuffOn(UPlayerInformData* PID)
{
	DebuffMaxHP = PID->MaxHealth * 0.2;
	PID->MaxHealth = PID->MaxHealth - DebuffMaxHP;

	DeBuffRemainDuration = DeBuffDuration;
	CheckDeBuff = true;
}

void UDPForcedHealing::DeBuffOff(UPlayerInformData* PID)
{
	PID->MaxHealth = PID->MaxHealth + DebuffMaxHP;
}

void UDPForcedHealing::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		CurrentCoolTime = DopingCoolTime; // 쿨타임 시작
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("ForcedHealing Use"));
		//효과


		//
	}
}

