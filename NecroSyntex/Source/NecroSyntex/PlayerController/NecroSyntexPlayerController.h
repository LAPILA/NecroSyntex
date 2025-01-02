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
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDMatchCountdown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
private:
	UPROPERTY()
	class ANecroSyntexHud* NecroSyntexHUD;

	// To Do : Make Set Match Time func
	// Test Match Time - 1h
	float MatchTime = 3600.0f;
	int32 PreviousMilliseconds;
	uint32 CountdownInt = 0;
};
