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
	PIDCheck = PID;

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
	
	if (OneKeyDoping->Able == true) {
		OneKeyDoping->UseDopingItem(PID);
	}

}

void UDopingComponent::SecondDopingUse() {
	if (TwoKeyDoping->Able == true) {
		TwoKeyDoping->UseDopingItem(PID);
	}

}

// Called every frame
void UDopingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OneKeyBool) {
		One_DopingCoolTime = OneKeyDoping->DopingCoolTime;
		One_DopingItemNum = OneKeyDoping->DopingItemNum;
		One_BuffDuration = OneKeyDoping->BuffDuration;
		One_Able = OneKeyDoping->Able;
	}

	if (TwoKeyBool) {
		Two_DopingCoolTime = TwoKeyDoping->DopingCoolTime;
		Two_DopingItemNum = TwoKeyDoping->DopingItemNum;
		Two_BuffDuration = TwoKeyDoping->BuffDuration;
		Two_Able = TwoKeyDoping->Able;
	}

	if (PID != PIDCheck) {
		PID = PIDCheck;
	}
	// Player 정보 갱신
	MaxHealth = PID->MaxHealth;
	CurrentHealth = PID->CurrentHealth;
	AttackPointMag = PID->AttackPointMag;
	MoveSpeed = PID->MoveSpeed;
	RunningSpeed = PID->RunningSpeed;
	Defense = PID->Defense;
	Blurred = PID->Blurred;
	ROF = PID->ROF;

}

// NewObject 생성 시 메모리 관리
void UDopingComponent::SetDopingKey(UDopingParent*& DopingKey, int32 Num)
{
	if (DopingKey)
	{
		DopingKey->ConditionalBeginDestroy(); // 기존 객체 제거
	}

	switch (Num)
	{
	case 1: DopingKey = NewObject<UDPLegEnforce>(this, UDPLegEnforce::StaticClass()); break;
	case 2: DopingKey = NewObject<UDPReducePain>(this); break;
	case 3: DopingKey = NewObject<UDPSupremeStrength>(this); break;
	case 4: DopingKey = NewObject<UDPForcedHealing>(this); break;
	case 5: DopingKey = NewObject<UDPFinalEmber>(this); break;
	case 6: DopingKey = NewObject<UDPBurningFurnace>(this); break;
	case 7: DopingKey = NewObject<UDPSolidFortress>(this); break;
	case 8: DopingKey = NewObject<UDPPainless>(this); break;
	default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
	}
}

// SetFirstDopingKey와 SetSecondDopingKey를 간소화
void UDopingComponent::SetFirstDopingKey(int32 Num)
{
	SetDopingKey(OneKeyDoping, Num);
	OneKeyBool = true;
}

void UDopingComponent::SetSecondDopingKey(int32 Num)
{
	SetDopingKey(TwoKeyDoping, Num);
	TwoKeyBool = true;
}

UDopingComponent* UDopingComponent::GetDopingComponent()
{
	return this;
}