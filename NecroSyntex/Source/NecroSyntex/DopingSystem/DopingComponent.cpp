// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UDopingComponent::UDopingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	UE_LOG(LogTemp, Warning, TEXT("333333333333333333333333333333333333333333333333333333"));
}

void UDopingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDopingComponent, OneKeyDoping);
	DOREPLIFETIME(UDopingComponent, FirstDopingCode);
	DOREPLIFETIME(UDopingComponent, OneKeyBool);

	DOREPLIFETIME(UDopingComponent, TwoKeyDoping);
	DOREPLIFETIME(UDopingComponent, SecondDopingCode);
	DOREPLIFETIME(UDopingComponent, TwoKeyBool);

	DOREPLIFETIME(UDopingComponent, PID);
	DOREPLIFETIME(UDopingComponent, PIDCheck);

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

	DOREPLIFETIME(UDopingComponent, MaxHealth);
	DOREPLIFETIME(UDopingComponent, CurrentHealth);
	DOREPLIFETIME(UDopingComponent, MaxShield);
	DOREPLIFETIME(UDopingComponent, CurrentShield);
	DOREPLIFETIME(UDopingComponent, MoveSpeed);
	DOREPLIFETIME(UDopingComponent, RunningSpeed);
	DOREPLIFETIME(UDopingComponent, Rebound);
	DOREPLIFETIME(UDopingComponent, MLAtaackPoint);
	DOREPLIFETIME(UDopingComponent, Defense);
	DOREPLIFETIME(UDopingComponent, Blurred);
	DOREPLIFETIME(UDopingComponent, ROF);

	DOREPLIFETIME(UDopingComponent, GunDamage);
	DOREPLIFETIME(UDopingComponent, TotalDamage);

	DOREPLIFETIME(UDopingComponent, LegEnforce);
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
	DOREPLIFETIME(UDopingComponent, CurseofChaos);


	DOREPLIFETIME(UDopingComponent, DopingMode);


}

// Called when the game starts
void UDopingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{

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
		ParadoxofGuardianship = NewObject<UDPParadoxofGuardianship>(this);
		HallucinationShield = NewObject<UDPHallucinationShield>(this);
		HPconversion = NewObject<UDPHPconversion>(this);
		CurseofChaos = NewObject<UDPCurseofChaos>(this);

		//도핑 모드(아군에게 도핑을 줄지 나에게 줄지 설정)
		DopingMode = false;

		OneKeyBool = false;
		TwoKeyBool = false;


		MaxHealth = PID->MaxHealth;
		CurrentHealth = PID->CurrentHealth;
		MaxShield = PID->MaxShield;
		CurrentShield = PID->CurrentShield;
		MoveSpeed = PID->MoveSpeed;
		RunningSpeed = PID->RunningSpeed;
		//Reboud = PID->Reboud;
		Defense = PID->Defense;
		Blurred = PID->Blurred;
		ROF = PID->ROF;
		//ItemUseRate = PID->ItemUSeRate
		TotalDamage = GunDamage + PID->DopingDamageBuff;

		//임시로 도핑키 셋팅
		OneKeyDoping = NewObject<UDPLegEnforce>(this);
		FirstDopingCode = 1;
		OneKeyBool = true;

		TwoKeyDoping = NewObject<UDPReducePain>(this);
		SecondDopingCode = 2;
		TwoKeyBool = true;
	}

}


// Called every frame
void UDopingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (GetOwner()->HasAuthority())
	{
		if (OneKeyBool) {
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
		}

		if (PID != PIDCheck) {
			PID = PIDCheck;
		}
		// Player 정보 갱신

		MaxHealth = PID->MaxHealth;
		CurrentHealth = PID->CurrentHealth;
		MaxShield = PID->MaxShield;
		CurrentShield = PID->CurrentShield;
		MoveSpeed = PID->MoveSpeed;
		RunningSpeed = PID->RunningSpeed;
		MLAtaackPoint = PID->MLAtaackPoint;
		Rebound = PID->Rebound;
		Defense = PID->Defense;
		Blurred = PID->Blurred;
		ROF = PID->ROF;

		TotalDamage = GunDamage + PID->DopingDamageBuff;
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
}

void UDopingComponent::SetSecondDopingKey_Implementation(int32 Num)
{
	SetDopingKey(TwoKeyDoping, Num);
	SecondDopingCode = Num;
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

void UDopingComponent::FirstDopingUse_Implementation() {

	if (PID->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

	if (OneKeyDoping->Able == true) {
		OneKeyDoping->UseDopingItem(PID);
	}
}

void UDopingComponent::SecondDopingUse_Implementation() {

	if (PID->CurrentDoped >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("This Character is doped two Doping"));
		return;
	}

	if (TwoKeyDoping->Able == true) {
		TwoKeyDoping->UseDopingItem(PID);
	}

}

//아군에게 도핑주는 시스템 관련 함수
void UDopingComponent::DopingModeChange_Implementation()
{
	if (DopingMode == false) {
		DopingMode = true;
	}
	else {
		DopingMode = false;
	}
}

void UDopingComponent::FirstDopingForAlly_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("아군에게 도핑사용 1번 작동"));
	
	AActor* Owner = GetOwner();
	if (!Owner) return;

	//UE_LOG(LogTemp, Warning, TEXT("1"));

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(Owner);
	if (!OwnerCharacter) return;

	//UE_LOG(LogTemp, Warning, TEXT("2"));

	UCameraComponent* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent) return;

	//UE_LOG(LogTemp, Warning, TEXT("3"));

	FVector Start = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	FVector End = Start + (ForwardVector * 10000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	//UE_LOG(LogTemp, Warning, TEXT("4"));
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		//UE_LOG(LogTemp, Warning, TEXT("5"));
		APlayerCharacter* HitCharacter = Cast<APlayerCharacter>(HitResult.GetActor());
		if (HitCharacter) {
			//UE_LOG(LogTemp, Warning, TEXT("6"));
			switch (FirstDopingCode)
			{
			case 1: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); UE_LOG(LogTemp, Warning, TEXT("7ㄴ")); break;
			case 2: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->ParadoxofGuardianship->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->HallucinationShield->BuffOn(HitCharacter->UDC->PID); break;
			case 9: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); break;
			case 10: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 11: HitCharacter->UDC->HPconversion->BuffOn(HitCharacter->UDC->PID); break;
			case 12: HitCharacter->UDC->CurseofChaos->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

		}
	}
}

void UDopingComponent::SecondDopingForAlly_Implementation()
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
			case 1: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); UE_LOG(LogTemp, Warning, TEXT("7ㄴ")); break;
			case 2: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->ParadoxofGuardianship->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->HallucinationShield->BuffOn(HitCharacter->UDC->PID); break;
			case 9: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); break;
			case 10: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 11: HitCharacter->UDC->HPconversion->BuffOn(HitCharacter->UDC->PID); break;
			case 12: HitCharacter->UDC->CurseofChaos->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

		}
	}
}