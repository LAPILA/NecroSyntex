// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicMonsterAI.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool AttackTiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isSkillAttackTime;
};
