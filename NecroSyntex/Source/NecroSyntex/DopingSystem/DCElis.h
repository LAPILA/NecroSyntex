// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingComponent.h"
#include "DCElis.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDCElis : public UDopingComponent
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	void Passive_Start() override;

	void Passive_End() override;

};
