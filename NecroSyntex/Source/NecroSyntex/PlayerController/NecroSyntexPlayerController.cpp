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

void ANecroSyntexPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
}

//Player State HUD
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

//Weapon State HUD
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


//Player Score HUD
void ANecroSyntexPlayerController::SetHUDScore(float Score)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("Army Score : %d"), FMath::FloorToInt(Score));
		NecroSyntexHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(AmmoText));
	}
}

void ANecroSyntexPlayerController::SetHUDDefeats(int32 Defeats)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->DieAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("Die Count : %d"), Defeats);
		NecroSyntexHUD->CharacterOverlay->DieAmount->SetText(FText::FromString(DefeatsText));
	}
}

//Game State HUD
void ANecroSyntexPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Hours = FMath::FloorToInt(CountdownTime / 3600.0f);
		int32 Minutes = FMath::FloorToInt((CountdownTime - Hours * 3600.0f) / 60.0f);
		int32 Seconds = FMath::FloorToInt(CountdownTime - (Hours * 3600.0f + Minutes * 60.0f));
		int32 Milliseconds = FMath::RoundToInt((CountdownTime - FMath::FloorToFloat(CountdownTime)) * 1000.0f);

		FString CountdownText = FString::Printf(TEXT("%02d:%02d:%02d:%03d"), Hours, Minutes, Seconds, Milliseconds);

		NecroSyntexHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ANecroSyntexPlayerController::SetHUDTime()
{
	float CurrentTime = MatchTime - GetWorld()->GetTimeSeconds();
	uint32 SecondsLeft = FMath::CeilToInt(CurrentTime);
	float Milliseconds = FMath::Frac(CurrentTime) * 1000.0f;

	if (CountdownInt != SecondsLeft || PreviousMilliseconds != (int)Milliseconds)
	{
		SetHUDMatchCountdown(CurrentTime);
	}

	CountdownInt = SecondsLeft;
	PreviousMilliseconds = (int)Milliseconds;
}
