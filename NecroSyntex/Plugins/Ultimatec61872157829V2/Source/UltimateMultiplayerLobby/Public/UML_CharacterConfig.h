// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "UML_CharacterConfig.generated.h"

USTRUCT(BlueprintType)
struct FUML_AvailableCharacterConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	FString CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	TSubclassOf<ACharacter> CharacterClass;
};

USTRUCT(BlueprintType)
struct FUML_AvailableEmoteConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	FString EmoteName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	UAnimMontage* EmoteMontage;
};


UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API UUML_CharacterConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	TArray<FUML_AvailableCharacterConfig> AvailableCharacters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Character Config")
	TArray<FUML_AvailableEmoteConfig> AvailableEmotes;
};
