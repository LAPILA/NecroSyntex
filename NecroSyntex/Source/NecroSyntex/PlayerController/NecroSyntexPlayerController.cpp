// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexPlayerController.h"
#include "NecroSyntex/HUD/NecroSyntexHud.h"
#include "NecroSyntex/HUD\CharacterOverlay.h"
#include "Components\Image.h"
#include "Components/TextBlock.h"
#include "Necrosyntex\Character\PlayerCharacter.h"

void ANecroSyntexPlayerController::BeginPlay()
{
	Super::BeginPlay();
	NecroSyntexHUD = Cast<ANecroSyntexHud>(GetHUD());
}
void ANecroSyntexPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->HealthBar;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		UMaterialInstanceDynamic* HealthBarMaterial = Cast<UMaterialInstanceDynamic>(NecroSyntexHUD->CharacterOverlay->HealthBar->GetDynamicMaterial());
		if (HealthBarMaterial)
		{
			HealthBarMaterial->SetScalarParameterValue(FName("Percentage"), HealthPercent);
		}
	}
}
void ANecroSyntexPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->ShieldBar;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		UMaterialInstanceDynamic* ShieldBarMaterial = Cast<UMaterialInstanceDynamic>(NecroSyntexHUD->CharacterOverlay->ShieldBar->GetDynamicMaterial());
		if (ShieldBarMaterial)
		{
			ShieldBarMaterial->SetScalarParameterValue(FName("Percentage"), ShieldPercent);
		}
	}
}

void ANecroSyntexPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		NecroSyntexHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ANecroSyntexPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("/ %d"), Ammo);
		NecroSyntexHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ANecroSyntexPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	if (PlayerCharacter)
	{
		SetHUDHealth(PlayerCharacter->GetHealth(), PlayerCharacter->GetMaxHealth());
		SetHUDShield(PlayerCharacter->GetShield(), PlayerCharacter->GetMaxShield());
	}
}
