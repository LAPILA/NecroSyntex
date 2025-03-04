// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingComponent.h"
#include "DCDeks.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDCDeks : public UDopingComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	void Passive_Start() override;

	void Passive_End() override;

	void FirstDopingUse_Implementation() override;

	void SecondDopingUse_Implementation() override;
};
