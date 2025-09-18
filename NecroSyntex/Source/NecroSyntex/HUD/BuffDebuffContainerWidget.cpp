#include "BuffDebuffContainerWidget.h"
#include "BuffDebuffEntryWidget.h"
#include "Components/HorizontalBox.h"
#include "Engine/DataTable.h"

void UBuffDebuffContainerWidget::UpdateBuffs(const TArray<FActiveBuff>& ActiveBuffs, UDataTable* BuffDataTable)
{
	if (!BuffBox || !DebuffBox || !EntryWidgetClass || !BuffDataTable) return;

	// 기존에 표시되던 모든 아이콘을 깨끗하게 지웁니다.
	BuffBox->ClearChildren();
	DebuffBox->ClearChildren();

	// 서버로부터 받은 최신 버프 목록을 기반으로 아이콘을 다시 생성합니다.
	for (const FActiveBuff& Buff : ActiveBuffs)
	{
		const FBuffData* BuffData = BuffDataTable->FindRow<FBuffData>(Buff.BuffID, "");
		if (!BuffData) continue;

		UBuffDebuffEntryWidget* NewEntry = CreateWidget<UBuffDebuffEntryWidget>(this, EntryWidgetClass);
		if (NewEntry)
		{
			// 버프가 시작된 후 이미 경과된 시간을 계산하여 전달합니다.
			const float ElapsedTime = GetWorld()->GetTimeSeconds() - Buff.StartTime;
			NewEntry->InitializeEntry(BuffData->Icon, Buff.Duration, ElapsedTime);


			// 버프/디버프 종류에 따라 알맞은 박스에 추가합니다.
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