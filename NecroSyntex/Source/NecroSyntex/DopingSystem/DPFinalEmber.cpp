// Fill out your copyright notice in the Description page of Project Settings.


#include "DPFinalEmber.h"

UDPFinalEmber::UDPFinalEmber()
	:Super()
{
	BuffDuration = 15.0f;
	CheckBuff = false;
	CheckDeBuff = false;
}

void UDPFinalEmber::BuffOn(UPlayerInformData* PID)
{
	if (CheckBuff == false) {
		//체력 50% 버프
		HPBuffNum = PID->MaxHealth * 0.5f;
		PID->MaxHealth = PID->MaxHealth + HPBuffNum;

		//체력 전체 회복
		PID->CurrentHealth = PID->MaxHealth;

		//이동 속도 20% 증가
		WalkingBuffNum = PID->MoveSpeed * 0.2f;
		PID->MoveSpeed = PID->MoveSpeed + WalkingBuffNum;

		RunningBuffNum = PID->RunningSpeed * 0.2f;
		PID->RunningSpeed = PID->RunningSpeed + RunningBuffNum;

		//근접 공격 50% 증가
		MLAttackBuffNum = PID->MLAtaackPoint * 0.5f;
		PID->MLAtaackPoint = PID->MLAtaackPoint + MLAttackBuffNum;

		//도핑 공격 데미지 추가
		PID->DopingDamageBuff += 20.0f;

		PID->CurrentDoped += 1;


	}

	CheckBuff = true;
	StartBuff(PID);
}

void UDPFinalEmber::BuffOff(UPlayerInformData* PID)
{
	if (CheckBuff == true) {
		//최대 체력 원상복귀
		PID->MaxHealth = PID->MaxHealth - HPBuffNum;

		//이동 속도 원상 복귀
		PID->MoveSpeed = PID->MoveSpeed - WalkingBuffNum;

		PID->RunningSpeed = PID->RunningSpeed - RunningBuffNum;

		//근접 공격 원상복귀
		PID->MLAtaackPoint = PID->MLAtaackPoint - MLAttackBuffNum;

		//데미지 버프 원상복귀
		PID->DopingDamageBuff -= 20.0f;

		CheckBuff = false;
		DeBuffOn(PID);
	}
}

void UDPFinalEmber::DeBuffOn(UPlayerInformData* PID)
{
	PID->CurrentHealth = 10.0f;

	PID->CurrentDoped -= 1;

	//CheckDeBuff = true;
	//StartDeBuff(PID);



}

void UDPFinalEmber::DeBuffOff(UPlayerInformData* PID)
{

}


void UDPFinalEmber::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;


		UE_LOG(LogTemp, Warning, TEXT("FinalEmber Use"));
		//효과
		BuffOn(PID);
		//
		StartCooldown();
	}
}
