// Fill out your copyright notice in the Description page of Project Settings.


#include "DCElis.h"
#include "Camera/CameraComponent.h"


void UDCElis::BeginPlay()
{
	Super::BeginPlay();


	if (GetOwner()->HasAuthority())
	{
		PID->MaxHealth = 200;
		PID->CurrentHealth = PID->MaxHealth;
		PID->MoveSpeed = 550.0f;
		PID->RunningSpeed = 1000.0f;
		PID->MLAtaackPoint = 60.0f;
		PID->Defense = 20;
	}


}

void UDCElis::Elis_Passive_Start(APlayerCharacter* HitCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Elis Passive On"));

	PID->DopingDamageBuff += 10.0f;
	HitCharacter->UDC->PID->DopingDamageBuff += 10.0f;

	GetWorld()->GetTimerManager().SetTimer(
		PassiveTimerHandle,
		[this, HitCharacter]() {Elis_Passive_End(HitCharacter); },
		Passive_Duration,
		false
	);
}

void UDCElis::Elis_Passive_End(APlayerCharacter* HitCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Elis Passive OFF"));

	PID->DopingDamageBuff -= 10.0f;
	HitCharacter->UDC->PID->DopingDamageBuff -= 10.0f;
}

void UDCElis::FirstDopingForAlly_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("First Doping for Team"));

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
			case 1: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); UE_LOG(LogTemp, Warning, TEXT("7��")); break;
			case 2: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;
			}

			Elis_Passive_Start(HitCharacter);

		}
	}
}

void UDCElis::SecondDopingForAlly_Implementation()
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
			switch (SecondDopingCode)
			{
			case 1: HitCharacter->UDC->LegEnforce->BuffOn(HitCharacter->UDC->PID); break;
			case 2: HitCharacter->UDC->ReducePain->BuffOn(HitCharacter->UDC->PID); break;
			case 3: HitCharacter->UDC->SupremeStrength->BuffOn(HitCharacter->UDC->PID); break;
			case 4: HitCharacter->UDC->ForcedHealing->BuffOn(HitCharacter->UDC->PID); break;
			case 5: HitCharacter->UDC->FinalEmber->BuffOn(HitCharacter->UDC->PID); break;
			case 6: HitCharacter->UDC->BurningFurnace->BuffOn(HitCharacter->UDC->PID); break;
			case 7: HitCharacter->UDC->SolidFortress->BuffOn(HitCharacter->UDC->PID); break;
			case 8: HitCharacter->UDC->Painless->BuffOn(HitCharacter->UDC->PID); break;
			default: UE_LOG(LogTemp, Warning, TEXT("Invalid Doping Key Set!")); break;

			}

			Elis_Passive_Start(HitCharacter);

		}
	}
}