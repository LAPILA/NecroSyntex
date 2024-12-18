// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	// Health & Shield ImageBar
	UPROPERTY(meta = (BindWidget))
	class UImage* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UImage* ShieldBar;

	// UI Effect Animation
	UPROPERTY(meta = (BindWidget))
	UImage* UIHitEffect;

	UPROPERTY(meta = (BindWidget))
	UImage* UISkillPosEffect;

	// Weapon Ammo UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	// Score UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DieAmount;
};
