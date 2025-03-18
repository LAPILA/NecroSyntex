// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "DCElis.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDCElis : public UDopingComponent
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	void Elis_Passive_Start(APlayerCharacter* HitCharacter);

	void Elis_Passive_End(APlayerCharacter* HitCharacter);

	void FirstDopingForAlly_Implementation() override;

	void SecondDopingForAlly_Implementation() override;

};
