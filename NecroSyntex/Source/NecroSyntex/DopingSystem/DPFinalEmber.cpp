// Fill out your copyright notice in the Description page of Project Settings.


#include "DPFinalEmber.h"

UDPFinalEmber::UDPFinalEmber()
	:Super()
{
	BuffDuration = 15.0f;
	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPFinalEmber::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		//체력 50% 버프
		HPBuffNum = DopedPC->MaxHealth * 0.5f;
		DopedPC->MaxHealth = DopedPC->MaxHealth + HPBuffNum;

		//체력 전체 회복
		DopedPC->Health = DopedPC->MaxHealth;

		//이동 속도 20% 증가
		WalkingBuffNum = DopedPC->WalkSpeed * 0.2f;
		DopedPC->WalkSpeed = DopedPC->WalkSpeed + WalkingBuffNum;

		RunningBuffNum = DopedPC->RunningSpeed * 0.2f;
		DopedPC->RunningSpeed = DopedPC->RunningSpeed + RunningBuffNum;

		//근접 공격 50% 증가
		MLAttackBuffNum = DopedPC->MLAtaackPoint * 0.5f;
		DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint + MLAttackBuffNum;

		//도핑 공격 데미지 추가
		DopedPC->DopingDamageBuff += 20.0f;

		DopedPC->CurrentDoped += 1;

		CheckBuff = true;
	}
	StartBuff(DopedPC);
}

void UDPFinalEmber::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		//최대 체력 원상복귀
		DopedPC->MaxHealth = DopedPC->MaxHealth - HPBuffNum;

		//이동 속도 원상 복귀
		DopedPC->WalkSpeed = DopedPC->WalkSpeed - WalkingBuffNum;

		DopedPC->RunningSpeed = DopedPC->RunningSpeed - RunningBuffNum;

		//근접 공격 원상복귀
		DopedPC->MLAtaackPoint = DopedPC->MLAtaackPoint - MLAttackBuffNum;

		//데미지 버프 원상복귀
		DopedPC->DopingDamageBuff -= 20.0f;

		CheckBuff = false;
		DeBuffOn(DopedPC);
	}
}

void UDPFinalEmber::DeBuffOn(APlayerCharacter* DopedPC)
{
	DopedPC->Health = 10.0f;

	DopedPC->CurrentDoped -= 1;

	//CheckDeBuff = true;
	//StartDeBuff(DopedPC);



}

void UDPFinalEmber::DeBuffOff(APlayerCharacter* DopedPC)
{

}


void UDPFinalEmber::UseDopingItem(APlayerCharacter* DopedPC)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("FinalEmber Use"));
		//효과
		BuffOn(DopedPC);
		//
		StartCooldown();
	}
}
