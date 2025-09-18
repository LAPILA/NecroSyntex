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
	// �����̳� �������κ��� ȣ��Ǿ� �����ܰ� �ð� ������ �����ϴ� �Լ�
	void InitializeEntry(UTexture2D* InIcon, float InTotalDuration, float InElapsedTime);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> DurationBar;

protected:
	virtual void NativeConstruct() override;
	// �� ������ ȣ��Ǿ� ProgressBar�� �ε巴�� ������Ʈ�մϴ�.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float TotalDuration = 0.f;
	float ElapsedTime = 0.f;
};