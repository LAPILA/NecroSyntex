// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AC_Monster.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AAC_Monster : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector NoiseLocation;
};
