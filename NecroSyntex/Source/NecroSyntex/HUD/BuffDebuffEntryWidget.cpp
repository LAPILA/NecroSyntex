#include "BuffDebuffEntryWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UBuffDebuffEntryWidget::InitializeEntry(UTexture2D* InIcon, float InTotalDuration, float InStartTime)
{
	if (IconImage && InIcon)
	{
		IconImage->SetBrushFromTexture(InIcon);
	}

	TotalDuration = InTotalDuration;
	StartTime = InStartTime;

	if (DurationBar)
	{
		DurationBar->SetVisibility(TotalDuration > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("[Entry] Initialized -> Duration: %.1f, StartTime: %.1f"), TotalDuration, StartTime);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMsg);
	}
}

void UBuffDebuffEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsVolatile = true;
}

void UBuffDebuffEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GEngine)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float CalculatedElapsedTime = CurrentTime - StartTime;
		const float Ratio = 1.f - FMath::Clamp(CalculatedElapsedTime / TotalDuration, 0.f, 1.f);
		FString DebugMsg = FString::Printf(TEXT("[Entry Tick] Total: %.1f, Start: %.1f, Elapsed: %.1f, Ratio: %.2f"), TotalDuration, StartTime, CalculatedElapsedTime, Ratio);
		// 키 값을 12345로 고정하여 매 프레임 같은 줄에 덮어쓰도록 합니다.
		GEngine->AddOnScreenDebugMessage(12345, 0.f, FColor::Yellow, DebugMsg);
	}

	if (TotalDuration <= 0.f || !DurationBar) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = CurrentTime - StartTime;

	const float RemainingRatio = 1.f - FMath::Clamp(ElapsedTime / TotalDuration, 0.f, 1.f);
	DurationBar->SetPercent(RemainingRatio);
}