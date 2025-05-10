// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "PCVelis.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API APCVelis : public APlayerCharacter
{
	GENERATED_BODY()

private:
	APCVelis();
	
protected:
	virtual void BeginPlay() override;

};
