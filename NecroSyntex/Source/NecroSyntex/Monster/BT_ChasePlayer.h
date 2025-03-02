// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UBT_ChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBT_ChasePlayer();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseSpeed = 600.0f;
};
