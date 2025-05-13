// Fill out your copyright notice in the Description page of Project Settings.


#include "DPSupremeStrength.h"

UDPSupremeStrength::UDPSupremeStrength() :Super()
{
	BuffDuration = 6.0f;
	DeBuffDuration = 6.0f;

	DopingDamageBuffNum = 10.0f;

	CheckBuff = false;
	CheckDeBuff = false;

}

void UDPSupremeStrength::UseDopingItem(APlayerCharacter* DopedPC)
{
	UE_LOG(LogTemp, Warning, TEXT("SupremeStrength Use"));
	//효과
	BuffOn(DopedPC);

}

void UDPSupremeStrength::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		//MLAttackBuffNum = DopedPC->MLAtaackPoint;

		//DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint + MLAttackBuffNum;

		DopedPC->DopingDamageBuff += DopingDamageBuffNum;

		DopedPC->CurrentDoped += 1;

		CheckBuff = true;
	}
	StartBuff(DopedPC);

	DeBuffOn(DopedPC);
}

void UDPSupremeStrength::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		//DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint - MLAttackBuffNum;

		DopedPC->DopingDamageBuff -= DopingDamageBuffNum;

		CheckBuff = false;
	}
}

void UDPSupremeStrength::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		//BlurredDeBuffNum = 1;
		//DopedPC->Blurred = DopedPC->Blurred - BlurredDeBuffNum;

		DopedPC->SPStrengthDeBuffON();

		CheckDeBuff = true;
	}
	StartDeBuff(DopedPC);
}

void UDPSupremeStrength::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		//DopedPC->Blurred = DopedPC->Blurred + BlurredDeBuffNum;

		DopedPC->SPStrengthDeBuffOFF();

		DopedPC->CurrentDoped -= 1;

		CheckDeBuff = false;
	}
}