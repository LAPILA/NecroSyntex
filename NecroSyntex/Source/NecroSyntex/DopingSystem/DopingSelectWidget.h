// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DopingComponent.h"
#include "DopingSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDopingSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Doping")
	UDopingComponent* GetDopingComponent();

	UDopingComponent* UDC;

	UFUNCTION(BlueprintCallable)
	void FirstKeyDopingChoice(int32 num);

	UFUNCTION(BlueprintCallable)
	void SecondkeyDopingChoice(int32 num);

};
