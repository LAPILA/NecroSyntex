// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffDebuffWidget.h"
#include "BuffDebuffEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UBuffDebuffEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Setup icon and other properties
	void InitializeEntry(const FBuffDebuffInfo& Info);

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	// 추가 기능 확장 가능
	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* DurationBar; // 남은 시간 표현 등

	FTimerHandle ProgressUpdateTimer;
	float TotalDuration = 0.f;
	float ElapsedTime = 0.f;

	void StartDurationUpdate(float InDuration);
	void UpdateDurationProgress();
};
