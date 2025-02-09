// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UDopingComponent::UDopingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	UE_LOG(LogTemp, Warning, TEXT("333333333333333333333333333333333333333333333333333333"));
}


// Called when the game starts
void UDopingComponent::BeginPlay()
{
	Super::BeginPlay();

	PID = NewObject<UPlayerInformData>(this);
	PIDCheck = PID;

	//아군에게 도핑을 받기위한 도핑 오브젝트 생성
	LegEnforce = NewObject<UDPLegEnforce>(this);
	ReducePain = NewObject<UDPReducePain>(this);
	SupremeStrength = NewObject<UDPSupremeStrength>(this);
	ForcedHealing = NewObject<UDPForcedHealing>(this);
	FinalEmber = NewObject<UDPFinalEmber>(this);
	BurningFurnace = NewObject<UDPBurningFurnace>(this);
	SolidFortress = NewObject<UDPSolidFortress>(this);
	Painless = NewObject<UDPPainless>(this);

	//도핑 모드(아군에게 도핑을 줄지 나에게 줄지 설정)
	DopingMode = false;

	OneKeyBool = false;
	TwoKeyBool = false;


	MaxHealth = PID->MaxHealth;
	CurrentHealth = PID->CurrentHealth;
	MoveSpeed = PID->MoveSpeed;
	RunningSpeed = PID->RunningSpeed;
	//Reboud = PID->Reboud;
	Defense = PID->Defense;
	Blurred = PID->Blurred;
	ROF = PID->ROF;
	//ItemUseRate = PID->ItemUSeRate
	TotalDamage = GunDamage + PID->DopingDamageBuff;


	// ...11

	//임시로 도핑키 셋팅
	OneKeyDoping = NewObject<UDPLegEnforce>(this);
	FirstDopingCode = 1;
	OneKeyBool = true;

	TwoKeyDoping = NewObject<UDPReducePain>(this);
	SecondDopingCode = 2;
	TwoKeyBool = true;


}

void UDopingComponent::FirstDopingUse() {

	if (PID->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

	if (OneKeyDoping->Able == true) {
		OneKeyDoping->UseDopingItem(PID);
	}

}

void UDopingComponent::SecondDopingUse() {

	if (PID->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

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
	MoveSpeed = PID->MoveSpeed;
	RunningSpeed = PID->RunningSpeed;
	Defense = PID->Defense;
	Blurred = PID->Blurred;
	ROF = PID->ROF;

	TotalDamage = GunDamage + PID->DopingDamageBuff;

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
	case 1: DopingKey = NewObject<UDPLegEnforce>(this); break;
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

//패시브

void UDopingComponent::Passive_Start()
{

}

void UDopingComponent::Passive_End()
{

}

//아군에게 도핑주는 시스템 관련 함수
void UDopingComponent::DopingModeChange()
{
	if (DopingMode == false) {
		DopingMode = true;
	}
	else {
		DopingMode = false;
	}
}

void UDopingComponent::FirstDopingForAlly()
{
	UE_LOG(LogTemp, Warning, TEXT("아군에게 도핑사용 1번 작동"));
	
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Warning, TEXT("1"));

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(Owner);
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("2"));

	UCameraComponent* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent) return;

	UE_LOG(LogTemp, Warning, TEXT("3"));

	FVector Start = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	FVector End = Start + (ForwardVector * 10000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	UE_LOG(LogTemp, Warning, TEXT("4"));
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		UE_LOG(LogTemp, Warning, TEXT("5"));
		APlayerCharacter* HitCharacter = Cast<APlayerCharacter>(HitResult.GetActor());
		if (HitCharacter) {
			UE_LOG(LogTemp, Warning, TEXT("6"));
			switch (FirstDopingCode)
			{
			case 1: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); UE_LOG(LogTemp, Warning, TEXT("7ㄴ")); break;
			case 2: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

		}
	}
}

void UDopingComponent::SecondDopingForAlly()
{
	UE_LOG(LogTemp, Warning, TEXT("아군에게 도핑사용 2번 작동 시작"));
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(Owner);
	if (!OwnerCharacter) return;

	UCameraComponent* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent) return;

	FVector Start = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	FVector End = Start + (ForwardVector * 10000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		APlayerCharacter* HitCharacter = Cast<APlayerCharacter>(HitResult.GetActor());
		if (HitCharacter) {
			switch (SecondDopingCode)
			{
			case 1: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); UE_LOG(LogTemp, Warning, TEXT("7ㄴ")); break;
			case 2: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

		}
	}
}