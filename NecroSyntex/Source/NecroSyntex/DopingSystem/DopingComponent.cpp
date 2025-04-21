// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UDopingComponent::UDopingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

void UDopingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDopingComponent, FirstDopingCode);
	DOREPLIFETIME(UDopingComponent, OneKeyBool);

	DOREPLIFETIME(UDopingComponent, SecondDopingCode);
	DOREPLIFETIME(UDopingComponent, TwoKeyBool);

	DOREPLIFETIME(UDopingComponent, Passive_Duration);

	DOREPLIFETIME(UDopingComponent, One_DopingItemNum);
	DOREPLIFETIME(UDopingComponent, One_DopingCoolTime);
	DOREPLIFETIME(UDopingComponent, One_BuffDuration);
	DOREPLIFETIME(UDopingComponent, One_DeBuffDuration);
	DOREPLIFETIME(UDopingComponent, One_CheckBuff);
	DOREPLIFETIME(UDopingComponent, One_CheckDeBuff);
	DOREPLIFETIME(UDopingComponent, One_Able);

	DOREPLIFETIME(UDopingComponent, Two_DopingItemNum);
	DOREPLIFETIME(UDopingComponent, Two_DopingCoolTime);
	DOREPLIFETIME(UDopingComponent, Two_BuffDuration);
	DOREPLIFETIME(UDopingComponent, Two_CheckBuff);
	DOREPLIFETIME(UDopingComponent, Two_CheckDeBuff);
	DOREPLIFETIME(UDopingComponent, Two_Able);

	DOREPLIFETIME(UDopingComponent, GunDamage);
	DOREPLIFETIME(UDopingComponent, TotalDamage);

	/*DOREPLIFETIME(UDopingComponent, LegEnforce);
	DOREPLIFETIME(UDopingComponent, ReducePain);
	DOREPLIFETIME(UDopingComponent, SupremeStrength);
	DOREPLIFETIME(UDopingComponent, ForcedHealing);
	DOREPLIFETIME(UDopingComponent, FinalEmber);
	DOREPLIFETIME(UDopingComponent, BurningFurnace);
	DOREPLIFETIME(UDopingComponent, SolidFortress);
	DOREPLIFETIME(UDopingComponent, Painless);
	DOREPLIFETIME(UDopingComponent, ParadoxofGuardianship);
	DOREPLIFETIME(UDopingComponent, HallucinationShield);
	DOREPLIFETIME(UDopingComponent, HPconversion);
	DOREPLIFETIME(UDopingComponent, CurseofChaos);*/


	DOREPLIFETIME(UDopingComponent, DopingforAllyMode);


}

// Called when the game starts
void UDopingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{

		//아군에게 도핑을 받기위한 도핑 오브젝트 생성
		LegEnforce = NewObject<UDPLegEnforce>(this);
		ReducePain = NewObject<UDPReducePain>(this);
		SupremeStrength = NewObject<UDPSupremeStrength>(this);
		ForcedHealing = NewObject<UDPForcedHealing>(this);
		FinalEmber = NewObject<UDPFinalEmber>(this);
		BurningFurnace = NewObject<UDPBurningFurnace>(this);
		SolidFortress = NewObject<UDPSolidFortress>(this);
		Painless = NewObject<UDPPainless>(this);
		ParadoxofGuardianship = NewObject<UDPParadoxofGuardianship>(this);
		HallucinationShield = NewObject<UDPHallucinationShield>(this);
		HPconversion = NewObject<UDPHPconversion>(this);
		CurseofChaos = NewObject<UDPCurseofChaos>(this);

		//도핑 모드(아군에게 도핑을 줄지 나에게 줄지 설정)
		DopingforAllyMode = false;

		OneKeyBool = false;
		TwoKeyBool = false;

		//임시로 도핑키 셋팅
		OneKeyDoping = HallucinationShield;
		One_DopingItemNum = OneKeyDoping->DopingItemNum;
		One_DopingCoolTime = OneKeyDoping->DopingCoolTime;
		FirstDopingCode = 1;
		OneKeyBool = true;
		One_Able = true;

		TwoKeyDoping = BurningFurnace;
		Two_DopingCoolTime = TwoKeyDoping->DopingCoolTime;
		Two_DopingItemNum = TwoKeyDoping->DopingItemNum;
		SecondDopingCode = 2;
		TwoKeyBool = true;
		Two_Able = true;
	}

}


// Called every frame
void UDopingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (GetOwner()->HasAuthority())
	{
		/*if (OneKeyBool) {
			One_DopingCoolTime = OneKeyDoping->DopingCoolTime;
			One_DopingItemNum = OneKeyDoping->DopingItemNum;
			One_BuffDuration = OneKeyDoping->BuffDuration;
			One_DeBuffDuration = OneKeyDoping->DeBuffDuration;
			One_CheckBuff = OneKeyDoping->CheckBuff;
			One_CheckDeBuff = OneKeyDoping->CheckDeBuff;
			One_Able = OneKeyDoping->Able;
		}

		if (TwoKeyBool) {
			Two_DopingCoolTime = TwoKeyDoping->DopingCoolTime;
			Two_DopingItemNum = TwoKeyDoping->DopingItemNum;
			Two_BuffDuration = TwoKeyDoping->BuffDuration;
			Two_DeBuffDuration = TwoKeyDoping->DeBuffDuration;
			Two_CheckBuff = TwoKeyDoping->CheckBuff;
			Two_CheckDeBuff = TwoKeyDoping->CheckDeBuff;
			Two_Able = TwoKeyDoping->Able;
		}*/
		// Player 정보 갱신

	}

}

void UDopingComponent::OnRep_OneAble()
{

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());

	UELOGCall();

	if (!OwnerCharacter) return;
	
	if (One_Able) {
		OwnerCharacter->SetHUDFirstDopingTrueicon();
	}
	else {
		OwnerCharacter->SetHUDFirstDopingFalseicon();
	}
}

void UDopingComponent::OnRep_TwoAble()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_TwoAble Call"));


	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (Two_Able) {
		OwnerCharacter->SetHUDSecondDopingTrueicon();
	}
	else {
		OwnerCharacter->SetHUDSecondDopingFalseicon();
	}
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
	case 1: DopingKey = SupremeStrength; break;
	case 2: DopingKey = BurningFurnace; break;
	case 3: DopingKey = Painless; break;
	case 4: DopingKey = FinalEmber; break;
	case 5: DopingKey = ReducePain; break;
	case 6: DopingKey = SolidFortress; break;
	case 7: DopingKey = ParadoxofGuardianship; break;
	case 8: DopingKey = HallucinationShield; break;
	case 9: DopingKey = LegEnforce; break;
	case 10: DopingKey = ForcedHealing; break;
	case 11: DopingKey = HPconversion; break;
	case 12: DopingKey = CurseofChaos; break;
	default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
	}
}

// SetFirstDopingKey와 SetSecondDopingKey를 간소화
void UDopingComponent::SetFirstDopingKey_Implementation(int32 Num)
{
	SetDopingKey(OneKeyDoping, Num);
	FirstDopingCode = Num;
	OneKeyBool = true;

	One_DopingCoolTime = OneKeyDoping->DopingCoolTime;
	One_DopingItemNum = OneKeyDoping->DopingItemNum;
	One_BuffDuration = OneKeyDoping->BuffDuration;
	One_DeBuffDuration = OneKeyDoping->DeBuffDuration;
	One_CheckBuff = OneKeyDoping->CheckBuff;
	One_CheckDeBuff = OneKeyDoping->CheckDeBuff;
	One_Able = true;
}

void UDopingComponent::SetSecondDopingKey_Implementation(int32 Num)
{
	SetDopingKey(TwoKeyDoping, Num);
	SecondDopingCode = Num;
	TwoKeyBool = true;

	Two_DopingCoolTime = TwoKeyDoping->DopingCoolTime;
	Two_DopingItemNum = TwoKeyDoping->DopingItemNum;
	Two_BuffDuration = TwoKeyDoping->BuffDuration;
	Two_DeBuffDuration = TwoKeyDoping->DeBuffDuration;
	Two_CheckBuff = TwoKeyDoping->CheckBuff;
	Two_CheckDeBuff = TwoKeyDoping->CheckDeBuff;
	Two_Able = true;
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

void UDopingComponent::PressedFirstDopingKey()
{
	//도핑 모드 확인
	if (GetOwner()->HasAuthority())
	{
		if (One_Able) {
			if (DopingforAllyMode) {
				FirstDopingForAlly();
			}
			else {
				FirstDopingUse();
			}
		}
	}
	else {
		ServerPressedFirstDopingKey();
	}
}

void UDopingComponent::ServerPressedFirstDopingKey_Implementation()
{
	if (One_Able) {
		if (DopingforAllyMode) {
			FirstDopingForAlly();
		}
		else {
			FirstDopingUse();
		}
	}
}

void UDopingComponent::PressedSecondDopingKey()
{
	//도핑 모드 확인
	if (GetOwner()->HasAuthority())
	{
		if (Two_Able) {
			if (DopingforAllyMode) {
				SecondDopingForAlly();
			}
			else {
				SecondDopingUse();
			}
		}
	
	}
	else {
		ServerPressedSecondDopingKey();
	}
}

void UDopingComponent::ServerPressedSecondDopingKey_Implementation()
{

	if (Two_Able) {
		if (DopingforAllyMode) {
			SecondDopingForAlly();
		}
		else {
			SecondDopingUse();
		}
	}

}



void UDopingComponent::FirstDopingUse() {

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

	OneKeyDoping->UseDopingItem(OwnerCharacter);
	OwnerCharacter->PlayDopingEffect();
	OwnerCharacter->PlayDopingMontage();
	ClientPlayDopingEffect();
	FirstDopingCoolStart();
}

void UDopingComponent::SecondDopingUse() {

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

	TwoKeyDoping->UseDopingItem(OwnerCharacter);
	OwnerCharacter->PlayDopingEffect();
	OwnerCharacter->PlayDopingMontage();
	ClientPlayDopingEffect();
	SecondDopingCoolStart();

}

void UDopingComponent::DopingModeChange()
{
	if (GetOwner()->HasAuthority()) {
		if (DopingforAllyMode == false) {
			DopingforAllyMode = true;
		}
		else {
			DopingforAllyMode = false;
		}
	}
	else {
		ServerDopingModeChange();
	}
}

//아군에게 도핑주는 시스템 관련 함수
void UDopingComponent::ServerDopingModeChange_Implementation()
{
	if (DopingforAllyMode == false) {
		DopingforAllyMode = true;
	}
	else {
		DopingforAllyMode = false;
	}
}


void UDopingComponent::FirstDopingForAlly()
{
	UE_LOG(LogTemp, Warning, TEXT("아군에게 도핑사용 1번 작동"));
	
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

			if (HitCharacter->CurrentDoped >= 2) {
				return;
			}

			switch (FirstDopingCode)
			{
			case 1: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter); UE_LOG(LogTemp, Warning, TEXT("7ㄴ")); break;
			case 2: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter); break;
			case 3: HitCharacter->UDC->Painless->BuffOn(HitCharacter); break;
			case 4: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter); break;
			case 5: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter); break;
			case 6: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter); break;
			case 7: HitCharacter->UDC->ParadoxofGuardianship->BuffOn(HitCharacter); break;
			case 8: HitCharacter->UDC->HallucinationShield->BuffOn(HitCharacter); break;
			case 9: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter); break;
			case 10: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter); break;
			case 11: HitCharacter->UDC->HPconversion->BuffOn(HitCharacter); break;
			case 12: HitCharacter->UDC->CurseofChaos->BuffOn(HitCharacter); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
			}

			HitCharacter->PlayDopingEffect();
			HitCharacter->UDC->ClientPlayDopingEffect();
			FirstDopingCoolStart();

			DopingforAllyMode = false;
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
		if (!HitCharacter || HitCharacter->CurrentDoped >= 2) {
			return;
		}


		if (HitCharacter) {
			switch (SecondDopingCode)
			{
			case 1: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter); break;
			case 2: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter); break;
			case 3: HitCharacter->UDC->Painless->BuffOn(HitCharacter); break;
			case 4: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter); break;
			case 5: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter); break;
			case 6: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter); break;
			case 7: HitCharacter->UDC->ParadoxofGuardianship->BuffOn(HitCharacter); break;
			case 8: HitCharacter->UDC->HallucinationShield->BuffOn(HitCharacter); break;
			case 9: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter); break;
			case 10: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter); break;
			case 11: HitCharacter->UDC->HPconversion->BuffOn(HitCharacter); break;
			case 12: HitCharacter->UDC->CurseofChaos->BuffOn(HitCharacter); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

			HitCharacter->PlayDopingEffect();
			HitCharacter->UDC->ClientPlayDopingEffect();
			SecondDopingCoolStart();

			DopingforAllyMode = false;
		}
	}
}


void UDopingComponent::FirstDopingCoolStart()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("First Doping CoolTime Start"));
	One_Able = false;

	OwnerCharacter->SetHUDFirstDopingFalseicon();

	GetWorld()->GetTimerManager().SetTimer(
		FirstDopingCoolTimehandle,
		[this]() { FirstDopingCoolEnd(); },
		One_DopingCoolTime,
		false
	);
}


void UDopingComponent::SecondDopingCoolStart()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	Two_Able = false;


	OwnerCharacter->SetHUDSecondDopingFalseicon();

	GetWorld()->GetTimerManager().SetTimer(
		SecondDopingCoolTimehandle,
		[this]() { SecondDopingCoolEnd(); },
		Two_DopingCoolTime,
		false
	);
}

void UDopingComponent::FirstDopingCoolEnd()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	One_Able = true;

	OwnerCharacter->SetHUDFirstDopingTrueicon();
}

void UDopingComponent::SecondDopingCoolEnd()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	Two_Able = true;

	OwnerCharacter->SetHUDSecondDopingTrueicon();
}

void UDopingComponent::UELOGCall()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_OneAble Call"));
}

void UDopingComponent::ClientPlayDopingEffect_Implementation()
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	OwnerCharacter->PlayDopingEffect();
}