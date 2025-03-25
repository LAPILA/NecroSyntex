// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{
	BuffDuration = 6.0f;
	DeBuffDuration = 6.0f;

	CheckBuff = false;
	CheckDeBuff = false;

}

void UDPSupremeStrength::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("SupremeStrength Use"));
		//효과
		BuffOn(DopedPC);

		//
		StartCooldown();
	}
}

void UDPSupremeStrength::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		MLAttackBuffNum = DopedPC->MLAtaackPoint;

		DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint + MLAttackBuffNum;

		DopedPC->CurrentDoped += 1;

		CheckBuff = true;
	}
	StartBuff(DopedPC);

	DeBuffOn(DopedPC);
}

void UDPSupremeStrength::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint - MLAttackBuffNum;

		CheckBuff = false;
	}
}

void UDPSupremeStrength::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		BlurredDeBuffNum = 1;
		DopedPC->Blurred = DopedPC->Blurred - BlurredDeBuffNum;

		CheckDeBuff = true;
	}
	StartDeBuff(DopedPC);
}

void UDPSupremeStrength::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		DopedPC->Blurred = DopedPC->Blurred + BlurredDeBuffNum;

		DopedPC->CurrentDoped -= 1;

		CheckDeBuff = false;
	}
}