// Fill out your copyright notice in the Description page of Project Settings.


#include "DCElis.h"
#include "Camera/CameraComponent.h"


void UDCElis::BeginPlay()
{
	Super::BeginPlay();

}

void UDCElis::Elis_Passive_Start(APlayerCharacter* HitCharacter)
{

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("Elis Passive On"));

	OwnerCharacter->DopingDamageBuff += 10.0f;
	HitCharacter->DopingDamageBuff += 10.0f;

	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this, HitCharacter]() {Elis_Passive_End(HitCharacter); },
		Passive_Duration,
		false
	);
}

void UDCElis::Elis_Passive_End(APlayerCharacter* HitCharacter)
{
	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("Elis Passive OFF"));

	OwnerCharacter->DopingDamageBuff -= 10.0f;
	HitCharacter->DopingDamageBuff -= 10.0f;
	passive_call = true;
}

void UDCElis::FirstDopingForAlly()
{
	UE_LOG(LogTemp, Warning, TEXT("First Doping for Team"));

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

			bool DopingUseSuccess;

			switch (FirstDopingCode)
			{
			case 1: DopingUseSuccess = HitCharacter->UDC->SupremeStrength->UseDopingItem(HitCharacter); break;
			case 2: DopingUseSuccess = HitCharacter->UDC->BurningFurnace->UseDopingItem(HitCharacter); break;
			case 3: DopingUseSuccess = HitCharacter->UDC->Painless->UseDopingItem(HitCharacter); break;
			case 4: DopingUseSuccess = HitCharacter->UDC->FinalEmber->UseDopingItem(HitCharacter); break;
			case 5: DopingUseSuccess = HitCharacter->UDC->ReducePain->UseDopingItem(HitCharacter); break;
			case 6: DopingUseSuccess = HitCharacter->UDC->SolidFortress->UseDopingItem(HitCharacter); break;
			case 7: DopingUseSuccess = HitCharacter->UDC->ParadoxofGuardianship->UseDopingItem(HitCharacter); break;
			case 8: DopingUseSuccess = HitCharacter->UDC->HallucinationShield->UseDopingItem(HitCharacter); break;
			case 9: DopingUseSuccess = HitCharacter->UDC->LegEnforce->UseDopingItem(HitCharacter); break;
			case 10: DopingUseSuccess = HitCharacter->UDC->ForcedHealing->UseDopingItem(HitCharacter); break;
			case 11: DopingUseSuccess = HitCharacter->UDC->HPconversion->UseDopingItem(HitCharacter); break;
			case 12: DopingUseSuccess = HitCharacter->UDC->CurseofChaos->UseDopingItem(HitCharacter); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
			}

			if (passive_call)
			{
				Elis_Passive_Start(HitCharacter);
				passive_call = false;
			}

			HitCharacter->PlayDopingEffect();

		}
	}
}

void UDCElis::SecondDopingForAlly()
{

	UE_LOG(LogTemp, Warning, TEXT("Second Doping for Team"));

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

			bool DopingUseSuccess;

			switch (SecondDopingCode)
			{
			case 1: DopingUseSuccess = HitCharacter->UDC->SupremeStrength->UseDopingItem(HitCharacter); break;
			case 2: DopingUseSuccess = HitCharacter->UDC->BurningFurnace->UseDopingItem(HitCharacter); break;
			case 3: DopingUseSuccess = HitCharacter->UDC->Painless->UseDopingItem(HitCharacter); break;
			case 4: DopingUseSuccess = HitCharacter->UDC->FinalEmber->UseDopingItem(HitCharacter); break;
			case 5: DopingUseSuccess = HitCharacter->UDC->ReducePain->UseDopingItem(HitCharacter); break;
			case 6: DopingUseSuccess = HitCharacter->UDC->SolidFortress->UseDopingItem(HitCharacter); break;
			case 7: DopingUseSuccess = HitCharacter->UDC->ParadoxofGuardianship->UseDopingItem(HitCharacter); break;
			case 8: DopingUseSuccess = HitCharacter->UDC->HallucinationShield->UseDopingItem(HitCharacter); break;
			case 9: DopingUseSuccess = HitCharacter->UDC->LegEnforce->UseDopingItem(HitCharacter); break;
			case 10: DopingUseSuccess = HitCharacter->UDC->ForcedHealing->UseDopingItem(HitCharacter); break;
			case 11: DopingUseSuccess = HitCharacter->UDC->HPconversion->UseDopingItem(HitCharacter); break;
			case 12: DopingUseSuccess = HitCharacter->UDC->CurseofChaos->UseDopingItem(HitCharacter); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
			}

			if (passive_call)
			{
				Elis_Passive_Start(HitCharacter);
				passive_call = false;
			}

			HitCharacter->PlayDopingEffect();
		}
	}
}