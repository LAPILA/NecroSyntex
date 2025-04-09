// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffDebuffEntryWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void UBuffDebuffEntryWidget::InitializeEntry(const FBuffDebuffInfo& Info)
{
	if (IconImage && Info.Icon)
	{
		IconImage->SetBrushFromTexture(Info.Icon);
	}

	if (DurationBar)
	{
		DurationBar->SetPercent(1.f);
		StartDurationUpdate(Info.Duration);
	}
}

void UBuffDebuffEntryWidget::StartDurationUpdate(float InDuration)
{
	if (!DurationBar) return;

	TotalDuration = InDuration;
	ElapsedTime = 0.f;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			ProgressUpdateTimer,
			this,
			&UBuffDebuffEntryWidget::UpdateDurationProgress,
			0.1f,
			true
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StartDurationUpdate: GetWorld() is null"));
	}
}


void UBuffDebuffEntryWidget::UpdateDurationProgress()
{
	ElapsedTime += 0.1f;

	if (DurationBar && TotalDuration > 0.f)
	{
		float RemainingRatio = 1.f - (ElapsedTime / TotalDuration);
		DurationBar->SetPercent(FMath::Clamp(RemainingRatio, 0.f, 1.f));
	}

	if (ElapsedTime >= TotalDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(ProgressUpdateTimer);
		UE_LOG(LogTemp, Warning, TEXT("BuffDebuff expired, removing from UI"));
	}
}


