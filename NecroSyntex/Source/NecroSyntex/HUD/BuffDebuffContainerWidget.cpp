// BuffDebuffContainerWidget.cpp

#include "BuffDebuffContainerWidget.h"
#include "BuffDebuffEntryWidget.h"
#include "Components/HorizontalBox.h"
#include "Engine/DataTable.h"

void UBuffDebuffContainerWidget::UpdateBuffs(const TArray<FActiveBuff>& ActiveBuffs, UDataTable* BuffDataTable)
{
	if (!BuffBox || !DebuffBox || !EntryWidgetClass || !BuffDataTable) return;

	BuffBox->ClearChildren();
	DebuffBox->ClearChildren();

	const FLinearColor BuffColor = FLinearColor(0.f, 1.f, 0.75f, 1.f);   // 민트색 (R=0, G=1, B=0.75)
	const FLinearColor DebuffColor = FLinearColor(1.f, 0.2f, 0.2f, 1.f); // 다홍색 (R=1, G=0.2, B=0.2)

	for (const FActiveBuff& Buff : ActiveBuffs)
	{
		const FBuffData* BuffData = BuffDataTable->FindRow<FBuffData>(Buff.BuffID, "");
		if (!BuffData) continue;

		UBuffDebuffEntryWidget* NewEntry = CreateWidget<UBuffDebuffEntryWidget>(this, EntryWidgetClass);
		if (NewEntry)
		{
			// bIsBuff 값에 따라 버프/디버프 색상을 선택
			const FLinearColor TargetColor = BuffData->bIsBuff ? BuffColor : DebuffColor;

			const float ElapsedTime = GetWorld()->GetTimeSeconds() - Buff.StartTime;

			// InitializeEntry 함수에 TargetColor를 함께 전달
			NewEntry->InitializeEntry(BuffData->Icon, Buff.Duration, Buff.StartTime, TargetColor);

			if (BuffData->bIsBuff)
			{
				BuffBox->AddChild(NewEntry);
			}
			else
			{
				DebuffBox->AddChild(NewEntry);
			}
		}
	}
}