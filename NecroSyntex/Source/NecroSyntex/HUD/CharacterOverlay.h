// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
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
	class UImage* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UImage* ShieldBar;

	UPROPERTY(meta = (BindWidget))
	UImage* UIHitEffect;

	UPROPERTY(meta = (BindWidget))
	UImage* UISkillPosEffect;
};
