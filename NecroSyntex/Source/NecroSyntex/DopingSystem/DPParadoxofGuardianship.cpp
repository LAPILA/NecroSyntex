// Fill out your copyright notice in the Description page of Project Settings.


#include "DPParadoxofGuardianship.h"

UDPParadoxofGuardianship::UDPParadoxofGuardianship()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;

	DefenseBuffNum = 10.0f;
	DefenseDeBuffNum = 20.0f;
	WalkingBuffNum = 350.0f;
	RunningBuffNum = 1000.0f;
	WalkingDeBuffNum = 150.0f;
	RunningDeBuffNum = 300.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPParadoxofGuardianship::ShiledCalcu(UPlayerInformData* PID)
{

	if (PID->CurrentShield > 0) {
		if (shieldOn == false) {
			if (isfirst == true) {
				PID->Defense = PID->Defense + DefenseBuffNum;
				PID->MoveSpeed = PID->MoveSpeed - WalkingDeBuffNum;
				PID->RunningSpeed = PID->RunningSpeed - RunningDeBuffNum;
				isfirst = false;
				shieldOn = true;
			}
			else {
				PID->Defense = PID->Defense + DefenseDeBuffNum;
				PID->MoveSpeed = PID->MoveSpeed - WalkingBuffNum;
				PID->RunningSpeed = PID->RunningSpeed - RunningBuffNum;
				PID->Defense = PID->Defense + DefenseBuffNum;
				PID->MoveSpeed = PID->MoveSpeed - WalkingDeBuffNum;
				PID->RunningSpeed = PID->RunningSpeed - RunningDeBuffNum;
				shieldOn = true;
			}
		}
	}
	else {
		if (shieldOn == true) {
			if (isfirst == true) {
				PID->Defense = PID->Defense - DefenseDeBuffNum;
				PID->MoveSpeed = PID->MoveSpeed + WalkingBuffNum;
				PID->RunningSpeed = PID->RunningSpeed + RunningBuffNum;
				isfirst = false;
				shieldOn = false;
			}
			else {
				PID->Defense = PID->Defense - DefenseBuffNum;
				PID->MoveSpeed = PID->MoveSpeed + WalkingDeBuffNum;
				PID->RunningSpeed = PID->RunningSpeed + RunningDeBuffNum;
				PID->Defense = PID->Defense - DefenseDeBuffNum;
				PID->MoveSpeed = PID->MoveSpeed + WalkingBuffNum;
				PID->RunningSpeed = PID->RunningSpeed + RunningBuffNum;
				shieldOn = false;
			}
		}
	}
}

void UDPParadoxofGuardianship::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;

		// Buff 적용 및 타이머 시작
		BuffOn(PID);

		// 쿨타임 시작
		StartCooldown();

		UE_LOG(LogTemp, Warning, TEXT("LegEnforce Use"));
	}
}

void UDPParadoxofGuardianship::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {

		isfirst = true;

		if (PID->CurrentShield > 0) {
			shieldOn = false;
		}
		else {
			shieldOn = true;
		}


		CheckBuff = true;
		GetWorld()->GetTimerManager().SetTimer(
			ShieldCalcuBuffTimer,
			[this, PID]() { ShiledCalcu(PID); },
			0.1f,
			true
		);

		StartBuff(PID);

	}
}

void UDPParadoxofGuardianship::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		GetWorld()->GetTimerManager().ClearTimer(ShieldCalcuBuffTimer);
		CheckBuff = false;
	}

	if (shieldOn == true) {
		PID->Defense = PID->Defense - DefenseBuffNum;
		PID->MoveSpeed = PID->MoveSpeed + WalkingDeBuffNum;
		PID->RunningSpeed = PID->RunningSpeed + RunningDeBuffNum;
	}
	else {
		PID->Defense = PID->Defense + DefenseDeBuffNum;
		PID->MoveSpeed = PID->MoveSpeed - WalkingBuffNum;
		PID->RunningSpeed = PID->RunningSpeed - RunningBuffNum;
	}

}

void UDPParadoxofGuardianship::DeBuffOn(UPlayerInformData* PID)
{

}

void UDPParadoxofGuardianship::DeBuffOff(UPlayerInformData* PID)
{

}