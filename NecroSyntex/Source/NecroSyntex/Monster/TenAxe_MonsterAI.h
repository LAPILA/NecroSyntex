// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicMonsterAI.h"
#include "TenAxe_MonsterAI.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API ATenAxe_MonsterAI : public ABasicMonsterAI
{
	GENERATED_BODY()
	
public:
	ATenAxe_MonsterAI();

	UFUNCTION(BlueprintCallable)
	void UpdateMaxWalkSpeed(float inputSpeed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ScreamMontage;

	UFUNCTION(BlueprintCallable)
	void PlayScreamAnimation();
	
};
