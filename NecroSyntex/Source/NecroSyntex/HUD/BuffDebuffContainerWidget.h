// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffDebuffContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UBuffDebuffContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UBuffDebuffEntryWidget* CreateEntryWidget(const FName& ID, const FBuffDebuffInfo& Info);

	void CreateAndTrackBuff(const FName& ID, const FBuffDebuffInfo& Info);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* BuffBox;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* DebuffBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBuffDebuffEntryWidget> EntryWidgetClass;

};
