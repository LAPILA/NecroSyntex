// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCurseofChaos.h"


UDPCurseofChaos::UDPCurseofChaos()
	: Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 2.0f;

	WalkingBuffNum = 350.0f;
	RunningBuffNum = 1000.0f;

	BuffRecoverAPS = 5.0f;

	CheckBuff = false;
	CheckDeBuff = false;

}

void UDPCurseofChaos::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;

		// Buff 적용 및 타이머 시작
		BuffOn(PID);
		// DeBuff 적용 및 타이머 시작
		DeBuffOn(PID);

		// 쿨타임 시작
		StartCooldown();

		UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos Use"));
	}
}

void UDPCurseofChaos::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {

		CheckBuff = true;

		PID->MoveSpeed += WalkingBuffNum;
		PID->RunningSpeed += RunningBuffNum;

		GetWorld()->GetTimerManager().SetTimer(
			HealingTimer,
			[this, PID]() { HealCharacter(PID); },
			1.0f,
			true
		);

		PID->CurrentDoped += 1;

	}

	StartBuff(PID);

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos BuffOn: Speed increased."));
}

void UDPCurseofChaos::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {

		PID->MoveSpeed -= WalkingBuffNum;
		PID->RunningSpeed -= RunningBuffNum;

		GetWorld()->GetTimerManager().ClearTimer(HealingTimer);
		CheckBuff = false;

	}

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos BuffOff: Speed normalized."));
}

void UDPCurseofChaos::DeBuffOn(UPlayerInformData* PID)
{

	if (CheckDeBuff == false) {

		CheckDeBuff = true;
	}

	StartDeBuff(PID);

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos DeBuffOn: Speed reduced."));
}

void UDPCurseofChaos::DeBuffOff(UPlayerInformData* PID)
{
	if (CheckDeBuff == true) {


		PID->CurrentDoped -= 1;

		CheckDeBuff = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos DeBuffOff: Speed restored."));
}

void UDPCurseofChaos::HealCharacter(UPlayerInformData* PID)
{
	if (PID->CurrentHealth < PID->MaxHealth) {
		if (PID->CurrentHealth + BuffRecoverAPS > PID->MaxHealth) {
			PID->CurrentHealth = PID->MaxHealth;
		}
		else {
			PID->CurrentHealth += BuffRecoverAPS;
		}
	}
}