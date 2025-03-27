// Fill out your copyright notice in the Description page of Project Settings.


#include "DPForcedHealing.h"
#include "TimerManager.h"

UDPForcedHealing::UDPForcedHealing() : Super()
{
	BuffDuration = 2.0f;
	DeBuffDuration = 6.0f;
	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPForcedHealing::HealCharacter(APlayerCharacter* DopedPC)
{
	if (DopedPC->Health < DopedPC->MaxHealth) {
		if (DopedPC->Health + BuffRecoverAPS > DopedPC->MaxHealth) {
			DopedPC->Health = DopedPC->MaxHealth;
		}
		else {
			DopedPC->Health += BuffRecoverAPS;
		}
	}

	DopedPC->UpdateHUDHealth();
}

void UDPForcedHealing::BuffOn(APlayerCharacter* DopedPC)
{

	if (CheckBuff == false) {
		
		targetRecover = (DopedPC->MaxHealth * 0.3);
		BuffRecoverAPS = targetRecover / 10.0f;

		DopedPC->CurrentDoped += 1;

		GetWorld()->GetTimerManager().SetTimer(
			HealingTimer,
			[this, DopedPC]() { HealCharacter(DopedPC); },
			0.2f,
			true
		);

		CheckBuff = true;
	}


	StartBuff(DopedPC);
}

void UDPForcedHealing::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		GetWorld()->GetTimerManager().ClearTimer(HealingTimer);
		CheckBuff = false;
	}


	DeBuffOn(DopedPC);
}

void UDPForcedHealing::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		DebuffMaxHP = DopedPC->MaxHealth * 0.2;
		DopedPC->MaxHealth = DopedPC->MaxHealth - DebuffMaxHP;

		CheckDeBuff = true;
	}

	StartDeBuff(DopedPC);
}

void UDPForcedHealing::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		DopedPC->MaxHealth = DopedPC->MaxHealth + DebuffMaxHP;
		CheckDeBuff = false;

		DopedPC->CurrentDoped -= 1;
	}

}

void UDPForcedHealing::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("ForcedHealing Use"));
		//효과
		BuffOn(DopedPC);

		//
		StartCooldown();
	}
}

