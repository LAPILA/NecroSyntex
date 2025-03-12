// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DopingComponent.h"
#include "DopingUI.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UDopingUI : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Doping")
	UDopingComponent* GetDopingComponent();

	UPROPERTY()
	UDopingComponent* UDC;

	UFUNCTION(BlueprintCallable, Category = "Doping")
	bool GetFirstDopingAble();

	UFUNCTION(BlueprintCallable, Category = "Doping")
	bool GetSecondDopingAble();
};
