// Fill out your copyright notice in the Description page of Project Settings.


#include "DPReducePain.h"

UDPReducePain::UDPReducePain()
	:Super()
{
	BuffDuration = 2.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

bool UDPReducePain::UseDopingItem(APlayerCharacter* DopedPC)
{
	UE_LOG(LogTemp, Warning, TEXT("ReducePain Use"));
	//효과
	BuffOn(DopedPC);

	return true;
}

void UDPReducePain::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		DefenseBuffNum = 50.0f - DopedPC->Defense;
		DopedPC->Defense = DopedPC->Defense + DefenseBuffNum;

		DopedPC->CurrentDoped += 1;

		CheckBuff = true;
		UE_LOG(LogTemp, Warning, TEXT("ReducePain Use"));
	}

	DeBuffOn(DopedPC);
	StartBuff(DopedPC);
}

void UDPReducePain::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		DopedPC->Defense = DopedPC->Defense - DefenseBuffNum;
		CheckBuff = false;
	}
	
}

void UDPReducePain::DeBuffOn(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == false) {
		DopedPC->Health = DopedPC->Health - (DopedPC->Health * 0.3);
		DopedPC->CurrentDoped -= 1;
	}
}

void UDPReducePain::DeBuffOff(APlayerCharacter* DopedPC)
{

}