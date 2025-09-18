#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffDebuffEntryWidget.generated.h"

class UImage;
class UProgressBar;
class UTexture2D;

UCLASS()
class NECROSYNTEX_API UBuffDebuffEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 컨테이너 위젯으로부터 호출되어 아이콘과 시간 정보를 설정하는 함수
	void InitializeEntry(UTexture2D* InIcon, float InTotalDuration, float InElapsedTime);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> DurationBar;

protected:
	virtual void NativeConstruct() override;
	// 매 프레임 호출되어 ProgressBar를 부드럽게 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float TotalDuration = 0.f;
	float ElapsedTime = 0.f;
};