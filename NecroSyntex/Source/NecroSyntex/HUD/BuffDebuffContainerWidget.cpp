#include "BuffDebuffContainerWidget.h"
#include "BuffDebuffEntryWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

UBuffDebuffEntryWidget* UBuffDebuffContainerWidget::CreateEntryWidget(const FName& ID, const FBuffDebuffInfo& Info)
{
	if (!EntryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("EntryWidgetClass is null!"));
		return nullptr;
	}

	UBuffDebuffEntryWidget* NewEntry = CreateWidget<UBuffDebuffEntryWidget>(GetWorld(), EntryWidgetClass);
	if (!NewEntry)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create BuffDebuffEntryWidget!"));
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("CreateEntryWidget: Created Entry Widget for ID: %s"), *ID.ToString());

	NewEntry->InitializeEntry(Info);

	if (Info.bIsBuff && BuffBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Entry to BuffBox"));
		BuffBox->AddChildToHorizontalBox(NewEntry);
	}
	else if (DebuffBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Entry to DebuffBox"));
		DebuffBox->AddChildToHorizontalBox(NewEntry);
	}

	return NewEntry;
}
