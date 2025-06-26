// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ServerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UServerGameInstance();

protected:
	virtual void Init() override;
};
