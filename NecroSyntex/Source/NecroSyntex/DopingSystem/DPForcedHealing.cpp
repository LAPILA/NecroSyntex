// Fill out your copyright notice in the Description page of Project Settings.


#include "DPForcedHealing.h"
#include "TimerManager.h"

UDPForcedHealing::UDPForcedHealing() : Super()
{
	BuffDuration = 2.0f;
	DeBuffDuration = 6.0f;
}

void UDPForcedHealing::HealCharacter(UPlayerInformData* PID)
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

void UDPForcedHealing::BuffOn(UPlayerInformData* PID)
{
	targetRecover = (PID->MaxHealth * 0.3);
	BuffRecoverAPS = targetRecover / (BuffDuration * 0.2f);

	GetWorld()->GetTimerManager().SetTimer(
		HealingTimer,
		[this, PID]() { HealCharacter(PID); },
		0.2f,
		true
	);

	PID->CurrentDoped += 1;

	CheckBuff = true;
	StartBuff(PID);
}

void UDPForcedHealing::BuffOff(UPlayerInformData* PID)
{
	GetWorld()->GetTimerManager().ClearTimer(HealingTimer);
	DeBuffOn(PID);
}

void UDPForcedHealing::DeBuffOn(UPlayerInformData* PID)
{
	DebuffMaxHP = PID->MaxHealth * 0.2;
	PID->MaxHealth = PID->MaxHealth - DebuffMaxHP;

	CheckDeBuff = true;
	StartDeBuff(PID);
}

void UDPForcedHealing::DeBuffOff(UPlayerInformData* PID)
{
	PID->MaxHealth = PID->MaxHealth + DebuffMaxHP;
	CheckDeBuff = false;

	PID->CurrentDoped -= 1;
}

void UDPForcedHealing::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("ForcedHealing Use"));
		//효과


		//
		StartCooldown();
	}
}

