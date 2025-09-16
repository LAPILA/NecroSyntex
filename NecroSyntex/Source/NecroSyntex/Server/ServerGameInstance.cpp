// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameInstance.h"
#include "NecroSyntex\Character\PlayerCharacter.h"

void UServerGameInstance::SaveCharacterSelected(TSubclassOf<APlayerCharacter> InCharacterClass)
{
	SelectedCharacterClass_Instance = InCharacterClass;

	return;
}

void UServerGameInstance::SaveDopingCode(int32 FirstDC, int32 SecondDC)
{
	FirstDopingCode_Instance = FirstDC;

	SecondDopingCode_Instance = SecondDC;

	return;
}
