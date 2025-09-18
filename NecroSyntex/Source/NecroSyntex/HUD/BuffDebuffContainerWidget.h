#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NecroSyntex/HUD/BuffDebuffTypes.h"
#include "BuffDebuffContainerWidget.generated.h"

class UDataTable;
class UHorizontalBox;
class UBuffDebuffEntryWidget;

UCLASS()
class NECROSYNTEX_API UBuffDebuffContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// BuffComponent�κ��� ȣ��Ǿ� UI ��ü�� ���ΰ�ħ�ϴ� ���� �Լ�
	void UpdateBuffs(const TArray<FActiveBuff>& ActiveBuffs, UDataTable* BuffDataTable);

	// BP���� ���ε��� ���� �����ܵ��� �� ���� �ڽ�
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BuffBox;

	// BP���� ���ε��� ����� �����ܵ��� �� ���� �ڽ�
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> DebuffBox;

	// BP���� ������ ���� ������ ��Ʈ�� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBuffDebuffEntryWidget> EntryWidgetClass;
};