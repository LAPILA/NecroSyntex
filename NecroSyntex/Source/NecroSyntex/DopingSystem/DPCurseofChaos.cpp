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

bool UDPCurseofChaos::UseDopingItem(APlayerCharacter* DopedPC)
{
	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos Use"));
	// Buff 적용 및 타이머 시작
	BuffOn(DopedPC);
	// DeBuff 적용 및 타이머 시작
	DeBuffOn(DopedPC);

	return true;
}

void UDPCurseofChaos::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {

		CheckBuff = true;

		DopedPC->WalkSpeed += WalkingBuffNum;
		DopedPC->RunningSpeed += RunningBuffNum;

		GetWorld()->GetTimerManager().SetTimer(
			HealingTimer,
			[this, DopedPC]() { HealCharacter(DopedPC); },
			1.0f,
			true
		);

		DopedPC->CurrentDoped += 1;

	}

	StartBuff(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos BuffOn: Speed increased."));
}

void UDPCurseofChaos::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {

		DopedPC->WalkSpeed -= WalkingBuffNum;
		DopedPC->RunningSpeed -= RunningBuffNum;

		GetWorld()->GetTimerManager().ClearTimer(HealingTimer);
		CheckBuff = false;

	}

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos BuffOff: Speed normalized."));
}

void UDPCurseofChaos::DeBuffOn(APlayerCharacter* DopedPC)
{

	if (CheckDeBuff == false) {

		CheckDeBuff = true;
		DopedPC->ReservedMoving = true;
	}

	StartDeBuff(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos DeBuffOn: Speed reduced."));
}

void UDPCurseofChaos::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {

		DopedPC->ReservedMoving = false;

		DopedPC->CurrentDoped -= 1;


		CheckDeBuff = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("UDPCurseofChaos DeBuffOff: Speed restored."));
}

void UDPCurseofChaos::HealCharacter(APlayerCharacter* DopedPC)
{
	if (DopedPC->Health < DopedPC->MaxHealth) {
		if (DopedPC->Health + BuffRecoverAPS > DopedPC->MaxHealth) {
			DopedPC->Health = DopedPC->MaxHealth;
		}
		else {
			DopedPC->Health += BuffRecoverAPS;
		}
	}
}