// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "ServerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UServerGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerCharacter> SelectedCharacterClass_Instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FirstDopingCode_Instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SecondDopingCode_Instance;
	

	UFUNCTION(BlueprintCallable)
	void SaveCharacterSelected(TSubclassOf<APlayerCharacter> InCharacterClass);

	UFUNCTION(BlueprintCallable)
	void SaveDopingCode(int32 FirstDC, int32 SecondDC);
};
