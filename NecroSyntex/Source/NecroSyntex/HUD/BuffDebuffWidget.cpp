#include "BuffDebuffWidget.h"
#include "BuffDebuffContainerWidget.h"
#include "BuffDebuffEntryWidget.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

void ABuffDebuffWidget::BeginPlay()
{
	Super::BeginPlay();

	if (BuffDebuffWidgetClass)
	{
		BuffDebuffWidget = CreateWidget<UBuffDebuffContainerWidget>(GetOwningPlayerController(), BuffDebuffWidgetClass);
		if (BuffDebuffWidget)
		{
			BuffDebuffWidget->AddToViewport();
		}
	}
	Test_AddMultipleBuffs();
}

void ABuffDebuffWidget::AddBuffDebuff(const FName& ID,FBuffDebuffInfo Info)
{
	if (!BuffDebuffWidget || BuffDebuffEntries.Contains(ID)) return;

	UBuffDebuffEntryWidget* Entry = BuffDebuffWidget->CreateEntryWidget(ID, Info);
	if (Entry)
	{
		BuffDebuffEntries.Add(ID, Entry);

		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(
			Timer,
			FTimerDelegate::CreateUObject(this, &ABuffDebuffWidget::OnBuffDebuffExpired, ID),
			Info.Duration,
			false
		);
		BuffDebuffTimers.Add(ID, Timer);
	}
}

void ABuffDebuffWidget::RemoveBuffDebuff(const FName& ID)
{
	if (!BuffDebuffEntries.Contains(ID)) return;

	if (UBuffDebuffEntryWidget* Entry = BuffDebuffEntries[ID])
	{
		Entry->RemoveFromParent();
	}
	BuffDebuffEntries.Remove(ID);

	if (BuffDebuffTimers.Contains(ID))
	{
		GetWorld()->GetTimerManager().ClearTimer(BuffDebuffTimers[ID]);
		BuffDebuffTimers.Remove(ID);
	}
}

void ABuffDebuffWidget::OnBuffDebuffExpired(FName ID)
{
	RemoveBuffDebuff(ID);
}

void ABuffDebuffWidget::Test_AddMultipleBuffs()
{
	if (!BuffDebuffWidget) return;

	// Buff 1
	FBuffDebuffInfo SpeedBuff;
	SpeedBuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icons_hexagon/7_hexagon"));
	SpeedBuff.Duration = 30.f;
	SpeedBuff.bIsBuff = true;
	AddBuffDebuff("SpeedBuff", SpeedBuff);

	// Buff 2
	FBuffDebuffInfo RegenBuff;
	RegenBuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icons_hexagon/2_accuracy"));
	RegenBuff.Duration = 20.f;
	RegenBuff.bIsBuff = true;
	AddBuffDebuff("RegenBuff", RegenBuff);

	// Debuff 1
	FBuffDebuffInfo PoisonDebuff;
	PoisonDebuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icons_hexagon/1_heal"));
	PoisonDebuff.Duration = 30.f;
	PoisonDebuff.bIsBuff = false;
	AddBuffDebuff("Poison", PoisonDebuff);

	// Debuff 2
	FBuffDebuffInfo SlowDebuff;
	SlowDebuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icons_hexagon/3_power"));
	SlowDebuff.Duration = 15.f;
	SlowDebuff.bIsBuff = false;
	AddBuffDebuff("Slow", SlowDebuff);
}
