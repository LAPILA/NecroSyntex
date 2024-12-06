// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecroSyntexPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ANecroSyntexPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void BeginPlay() override;
private:
	class ANecroSyntexHud* NecroSyntexHUD;
};
