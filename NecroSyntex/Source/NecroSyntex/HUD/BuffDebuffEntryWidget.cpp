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

	// ���ӽð��� 0���� ū ����(�ð��� ����)�� ��쿡�� ProgressBar�� ���̰� �մϴ�.
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

	// �ð��� ������ �ƴϰų� ProgressBar�� ������ �������� �ʽ��ϴ�.
	if (TotalDuration <= 0.f || !DurationBar) return;

	// ��� �ð��� �� ƽ ������Ʈ�մϴ�.
	ElapsedTime += InDeltaTime;

	// ���� �ð� ������ ����Ͽ� ProgressBar�� �ݿ��մϴ�.
	const float RemainingRatio = 1.f - FMath::Clamp(ElapsedTime / TotalDuration, 0.f, 1.f);
	DurationBar->SetPercent(RemainingRatio);
}