// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingComponent.h"

// Sets default values for this component's properties
UDopingComponent::UDopingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UDopingComponent::BeginPlay()
{
	Super::BeginPlay();

	PID = NewObject<UPlayerInformData>(this);

	//LE = NewObject<UDPLegEnforce>(this);
	//RP = NewObject<UDPReducePain>(this);
	//SS = NewObject<UDPSupremeStrength>(this);
	//FH = NewObject<UDPForcedHealing>(this);
	//FE = NewObject<UDPFinalEmber>(this);
	//BF = NewObject<UDPBurningFurnace>(this);
	//SF = NewObject<UDPSolidFortress>(this);
	//PL = NewObject<UDPPainless>(this);

	OneKeyBool = false;
	TwoKeyBool = false;


	MaxHealth = PID->MaxHealth;
	CurrentHealth = PID->CurrentHealth;
	AttackPointMag = PID->AttackPointMag;
	MoveSpeed = PID->MoveSpeed;
	RunningSpeed = PID->RunningSpeed;
	//Reboud = PID->Reboud;
	Defense = PID->Defense;
	Blurred = PID->Blurred;
	ROF = PID->ROF;
	//ItemUseRate = PID->ItemUSeRate


	//OneKeyDoping = LE; // 나중에는 도핑 선택창에서 플레이어가 도핑 고른 것으로 들어가게 할거임
	//TwoKeyDoping = RP;

	// ...11

}

void UDopingComponent::FirstDopingUse() {
	if (OneKeyDoping->Able) {
		OneKeyDoping->UseDopingItem(PID);
	}

}

void UDopingComponent::SecondDopingUse() {
	if (TwoKeyDoping->Able) {
		TwoKeyDoping->UseDopingItem(PID);
	}

}

void UDopingComponent::ReduceCooldown(float DeltaTime)
{
	if (OneKeyBool) {
		if (OneKeyDoping) {
			if (OneKeyDoping->CurrentCoolTime > 0.0f) {
				OneKeyDoping->CurrentCoolTime -= DeltaTime;
			}
			if (OneKeyDoping->CurrentCoolTime <= 0.0f && OneKeyDoping->Able == false) {
				OneKeyDoping->Able = true;
			}
		}
	}

	if (TwoKeyBool) {
		if (TwoKeyDoping) {
			if (TwoKeyDoping->CurrentCoolTime > 0.0f) {
				TwoKeyDoping->CurrentCoolTime -= DeltaTime;
			}
			if (TwoKeyDoping->CurrentCoolTime <= 0.0f && TwoKeyDoping->Able == false) {
				TwoKeyDoping->Able = true;
			}
		}
	}

}

void UDopingComponent::ReduceBuffDuration(float DeltaTime)
{
	if (OneKeyBool) {
		if (OneKeyDoping) {
			if (OneKeyDoping->BuffRemainDuration > 0.0f) {
				OneKeyDoping->BuffRemainDuration -= DeltaTime;
			}
			if (OneKeyDoping->BuffRemainDuration <= 0.0f && One_CheckBuff == true) {
				OneKeyDoping->CheckBuff = false;
				OneKeyDoping->BuffOff(PID);
			}
		}
	}

	if (TwoKeyBool) {
		if (TwoKeyDoping) {
			if (TwoKeyDoping->BuffRemainDuration > 0.0f) {
				TwoKeyDoping->BuffRemainDuration -= DeltaTime;
			}
			if (TwoKeyDoping->BuffRemainDuration <= 0.0f && Two_CheckBuff == true) {
				TwoKeyDoping->CheckBuff = false;
				TwoKeyDoping->BuffOff(PID);
			}
		}
	}
}

void UDopingComponent::ReduceDeBuffDuration(float DeltaTime)
{
	if (OneKeyBool) {
		if (OneKeyDoping) {
			if (OneKeyDoping->DeBuffRemainDuration > 0.0f) {
				OneKeyDoping->DeBuffRemainDuration -= DeltaTime;
			}
			if (OneKeyDoping->DeBuffRemainDuration <= 0.0f && One_CheckDeBuff == true) {
				OneKeyDoping->CheckDeBuff = false;
				OneKeyDoping->DeBuffOff(PID);
			}
		}
	}

	if (TwoKeyBool) {
		if (TwoKeyDoping) {
			if (TwoKeyDoping->DeBuffRemainDuration > 0.0f) {
				TwoKeyDoping->DeBuffRemainDuration -= DeltaTime;
			}
			if (TwoKeyDoping->DeBuffRemainDuration <= 0.0f && Two_CheckDeBuff == true) {
				TwoKeyDoping->CheckDeBuff = false;
				TwoKeyDoping->DeBuffOff(PID);
			}
		}
	}
}


// Called every frame
void UDopingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (OneKeyBool) {
		One_CurrentCoolTime = OneKeyDoping->CurrentCoolTime;
		One_DopingCoolTime = OneKeyDoping->DopingCoolTime;
		One_DopingItemNum = OneKeyDoping->DopingItemNum;
		One_BuffDuration = OneKeyDoping->BuffDuration;
		One_BuffRemainDuration = OneKeyDoping->BuffRemainDuration;
		One_DeBuffDuration = OneKeyDoping->DeBuffDuration;
		One_DeBuffRemainDuration = OneKeyDoping->DeBuffRemainDuration;
		One_CheckBuff = OneKeyDoping->CheckBuff;
		One_CheckDeBuff = OneKeyDoping->CheckDeBuff;
		One_Able = OneKeyDoping->Able;
	}

	if (TwoKeyBool) {
		Two_CurrentCoolTime = TwoKeyDoping->CurrentCoolTime;
		Two_DopingCoolTime = TwoKeyDoping->DopingCoolTime;
		Two_DopingItemNum = TwoKeyDoping->DopingItemNum;
		Two_BuffDuration = TwoKeyDoping->BuffDuration;
		Two_BuffRemainDuration = TwoKeyDoping->BuffRemainDuration;
		Two_DeBuffDuration = TwoKeyDoping->DeBuffDuration;
		Two_DeBuffRemainDuration = TwoKeyDoping->DeBuffRemainDuration;
		Two_CheckBuff = TwoKeyDoping->CheckBuff;
		Two_CheckDeBuff = TwoKeyDoping->CheckDeBuff;
		Two_Able = TwoKeyDoping->Able;
	}

	//OneKeyDoping->GetDeltaTime(DeltaTime);
	//TwoKeyDoping->GetDeltaTime(DeltaTime);

	PID->CurrentHealth += PID->RecoverAPS;

	MaxHealth = PID->MaxHealth;
	CurrentHealth = PID->CurrentHealth;
	AttackPointMag = PID->AttackPointMag;
	MoveSpeed = PID->MoveSpeed;
	RunningSpeed = PID->RunningSpeed;
	//Reboud = PID->Reboud;
	Defense = PID->Defense;
	Blurred = PID->Blurred;
	ROF = PID->ROF;
	//ItemUseRate = PID->ItemUSeRate

	// ...
	if (One_Able == false || Two_Able == false) {
		ReduceCooldown(DeltaTime);
	}

	if (One_CheckBuff == true || Two_CheckBuff == true) {
		ReduceBuffDuration(DeltaTime);
	}

	if (One_CheckDeBuff == true || Two_CheckDeBuff == true) {
		ReduceDeBuffDuration(DeltaTime);
	}

}

void UDopingComponent::SetFirstDopingKey(int32 Num)
{
	switch (Num)
	{
	case 1:
		OneKeyDoping = NewObject<UDPLegEnforce>(this);
		OneKeyBool = true;
		break;
	case 2:
		OneKeyDoping = NewObject<UDPReducePain>(this);
		OneKeyBool = true;
		break;
	case 3:
		OneKeyDoping = NewObject<UDPSupremeStrength>(this);
		OneKeyBool = true;
		break;
	case 4:
		OneKeyDoping = NewObject<UDPForcedHealing>(this);
		OneKeyBool = true;
		break;
	case 5:
		OneKeyDoping = NewObject<UDPFinalEmber>(this);
		OneKeyBool = true;
		break;
	case 6:
		OneKeyDoping = NewObject<UDPBurningFurnace>(this);
		OneKeyBool = true;
		break;
	case 7:
		OneKeyDoping = NewObject<UDPSolidFortress>(this);
		OneKeyBool = true;
		break;
	case 8:
		OneKeyDoping = NewObject<UDPPainless>(this);
		OneKeyBool = true;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("키셋팅 실패"));
		break;
	}
}

void UDopingComponent::SetSecondDopingKey(int32 Num)
{
	switch (Num)
	{
	case 1:
		TwoKeyDoping = NewObject<UDPLegEnforce>(this);
		TwoKeyBool = true;
		break;
	case 2:
		TwoKeyDoping = NewObject<UDPReducePain>(this);
		TwoKeyBool = true;
		break;
	case 3:
		TwoKeyDoping = NewObject<UDPSupremeStrength>(this);
		TwoKeyBool = true;
		break;
	case 4:
		TwoKeyDoping = NewObject<UDPForcedHealing>(this);
		TwoKeyBool = true;
		break;
	case 5:
		TwoKeyDoping = NewObject<UDPFinalEmber>(this);
		TwoKeyBool = true;
		break;
	case 6:
		TwoKeyDoping = NewObject<UDPBurningFurnace>(this);
		TwoKeyBool = true;
		break;
	case 7:
		TwoKeyDoping = NewObject<UDPSolidFortress>(this);
		TwoKeyBool = true;
		break;
	case 8:
		TwoKeyDoping = NewObject<UDPPainless>(this);
		TwoKeyBool = true;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("키셋팅 실패"));
		break;
	}
}

