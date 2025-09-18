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
	// BuffComponent로부터 호출되어 UI 전체를 새로고침하는 메인 함수
	void UpdateBuffs(const TArray<FActiveBuff>& ActiveBuffs, UDataTable* BuffDataTable);

	// BP에서 바인딩할 버프 아이콘들이 들어갈 가로 박스
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BuffBox;

	// BP에서 바인딩할 디버프 아이콘들이 들어갈 가로 박스
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> DebuffBox;

	// BP에서 생성할 개별 아이콘 엔트리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBuffDebuffEntryWidget> EntryWidgetClass;
};