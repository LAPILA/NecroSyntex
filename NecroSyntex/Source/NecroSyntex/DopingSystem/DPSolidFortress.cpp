// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSolidFortress.h"

UDPSolidFortress::UDPSolidFortress()
	:Super()
{
	BuffDuration = 10.0f;
	DeBuffDuration = 10.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

bool UDPSolidFortress::UseDopingItem(APlayerCharacter* DopedPC)
{
	UE_LOG(LogTemp, Warning, TEXT("SolidFortress Use"));
	//효과
	BuffOn(DopedPC);

	return true;
}

void UDPSolidFortress::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		HPBuffNum = DopedPC->MaxHealth * 0.5f;

		DopedPC->MaxHealth = DopedPC->MaxHealth + HPBuffNum;

		DopedPC->CurrentDoped += 1;


		CheckBuff = true;
	}
	StartBuff(DopedPC);

	DeBuffOn(DopedPC);
}

void UDPSolidFortress::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		DopedPC->MaxHealth = DopedPC->MaxHealth - HPBuffNum;
		CheckBuff = false;
	}
}

void UDPSolidFortress::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		WalkingDeBuffNum = DopedPC->WalkSpeed * 0.2f;
		RunningDeBuffNum = DopedPC->RunningSpeed * 0.2f;

		DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingDeBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningDeBuffNum;
		DopedPC->DopingDamageBuff -= 20.0f;

		CheckDeBuff = true;
	}
	StartDeBuff(DopedPC);
}

void UDPSolidFortress::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingDeBuffNum;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningDeBuffNum;
		DopedPC->DopingDamageBuff += 20.0f;

		DopedPC->CurrentDoped -= 1;
		CheckDeBuff = false;
	}

}
