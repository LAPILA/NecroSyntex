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
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShieldBar;

	// UI Effect Animation
	// To Do : nedd to make working this
	UPROPERTY(meta = (BindWidget))
	UImage* UIHitEffect;

	UPROPERTY(meta = (BindWidget))
	UImage* UISkillPosEffect;

	// Weapon UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	// Score UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DieAmount;

	// Game Info UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesText;

	// Net Info UI

	UPROPERTY(meta = (BindWidget))
	UImage* HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation;

	UPROPERTY(meta = (BindWidget))
	class UBuffDebuffContainerWidget* BuffDebuffContainer;
};
