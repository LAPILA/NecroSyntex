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

void UDPParadoxofGuardianship::ShiledCalcu(APlayerCharacter* DopedPC)
{

	if (DopedPC->Shield > 0) {
		if (shieldOn == false) {
			if (isfirst == true) {
				DopedPC->Defense = DopedPC->Defense + DefenseBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingDeBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningDeBuffNum;
				isfirst = false;
				shieldOn = true;
			}
			else {
				DopedPC->Defense = DopedPC->Defense + DefenseDeBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningBuffNum;
				DopedPC->Defense = DopedPC->Defense + DefenseBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingDeBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningDeBuffNum;
				shieldOn = true;
			}
		}
	}
	else {
		if (shieldOn == true) {
			if (isfirst == true) {
				DopedPC->Defense = DopedPC->Defense - DefenseDeBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningBuffNum;
				isfirst = false;
				shieldOn = false;
			}
			else {
				DopedPC->Defense = DopedPC->Defense - DefenseBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingDeBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningDeBuffNum;
				DopedPC->Defense = DopedPC->Defense - DefenseDeBuffNum;
				DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingBuffNum;
				DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningBuffNum;
				shieldOn = false;
			}
		}
	}
}

bool UDPParadoxofGuardianship::UseDopingItem(APlayerCharacter* DopedPC)
{
	// Buff 적용 및 타이머 시작
	BuffOn(DopedPC);

	return true;

}

void UDPParadoxofGuardianship::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {

		isfirst = true;

		if (DopedPC->Shield > 0) {
			shieldOn = false;
		}
		else {
			shieldOn = true;
		}


		CheckBuff = true;
		GetWorld()->GetTimerManager().SetTimer(
			ShieldCalcuBuffTimer,
			[this, DopedPC]() { ShiledCalcu(DopedPC); },
			0.1f,
			true
		);

		StartBuff(DopedPC);

	}
}

void UDPParadoxofGuardianship::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		GetWorld()->GetTimerManager().ClearTimer(ShieldCalcuBuffTimer);
		CheckBuff = false;
	}

	if (shieldOn == true) {
		DopedPC->Defense = DopedPC->Defense - DefenseBuffNum;
		DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingDeBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningDeBuffNum;
	}
	else {
		DopedPC->Defense = DopedPC->Defense + DefenseDeBuffNum;
		DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningBuffNum;
	}

}

void UDPParadoxofGuardianship::DeBuffOn(APlayerCharacter* DopedPC)
{

}

void UDPParadoxofGuardianship::DeBuffOff(APlayerCharacter* DopedPC)
{

}