#include "BuffDebuffContainerWidget.h"
#include "BuffDebuffEntryWidget.h"
#include "Components/HorizontalBox.h"
#include "Engine/DataTable.h"

void UBuffDebuffContainerWidget::UpdateBuffs(const TArray<FActiveBuff>& ActiveBuffs, UDataTable* BuffDataTable)
{
	if (!BuffBox || !DebuffBox || !EntryWidgetClass || !BuffDataTable) return;

	// ������ ǥ�õǴ� ��� �������� �����ϰ� ����ϴ�.
	BuffBox->ClearChildren();
	DebuffBox->ClearChildren();

	// �����κ��� ���� �ֽ� ���� ����� ������� �������� �ٽ� �����մϴ�.
	for (const FActiveBuff& Buff : ActiveBuffs)
	{
		const FBuffData* BuffData = BuffDataTable->FindRow<FBuffData>(Buff.BuffID, "");
		if (!BuffData) continue;

		UBuffDebuffEntryWidget* NewEntry = CreateWidget<UBuffDebuffEntryWidget>(this, EntryWidgetClass);
		if (NewEntry)
		{
			// ������ ���۵� �� �̹� ����� �ð��� ����Ͽ� �����մϴ�.
			const float ElapsedTime = GetWorld()->GetTimeSeconds() - Buff.StartTime;
			NewEntry->InitializeEntry(BuffData->Icon, Buff.Duration, ElapsedTime);


			// ����/����� ������ ���� �˸��� �ڽ��� �߰��մϴ�.
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