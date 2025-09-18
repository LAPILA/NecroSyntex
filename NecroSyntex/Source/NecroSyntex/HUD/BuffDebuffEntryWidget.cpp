#include "BuffDebuffEntryWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UBuffDebuffEntryWidget::InitializeEntry(UTexture2D* InIcon, float InTotalDuration, float InElapsedTime)
{
	if (IconImage && InIcon)
	{
		IconImage->SetBrushFromTexture(InIcon);
	}

	TotalDuration = InTotalDuration;
	ElapsedTime = InElapsedTime;

	// 지속시간이 0보다 큰 버프(시간제 버프)일 경우에만 ProgressBar를 보이게 합니다.
	if (DurationBar)
	{
		DurationBar->SetVisibility(TotalDuration > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
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

	// 시간제 버프가 아니거나 ProgressBar가 없으면 실행하지 않습니다.
	if (TotalDuration <= 0.f || !DurationBar) return;

	// 경과 시간을 매 틱 업데이트합니다.
	ElapsedTime += InDeltaTime;

	// 남은 시간 비율을 계산하여 ProgressBar에 반영합니다.
	const float RemainingRatio = 1.f - FMath::Clamp(ElapsedTime / TotalDuration, 0.f, 1.f);
	DurationBar->SetPercent(RemainingRatio);
}