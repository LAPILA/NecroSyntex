// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BuffDebuffWidget.generated.h"

class UBuffDebuffContainerWidget;
class UBuffDebuffEntryWidget;
class UTexture2D;

USTRUCT(BlueprintType)
struct FBuffDebuffInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBuff;
};

UCLASS()
class NECROSYNTEX_API ABuffDebuffWidget : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// Show new Buff or Debuff
	UFUNCTION(BlueprintCallable)
	void AddBuffDebuff(const FName& ID, FBuffDebuffInfo Info);

	// Remove Buff or Debuff manually
	UFUNCTION(BlueprintCallable)
	void RemoveBuffDebuff(const FName& ID);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBuffDebuffContainerWidget> BuffDebuffWidgetClass;

	UPROPERTY()
	UBuffDebuffContainerWidget* BuffDebuffWidget;

private:
	// 타이머 관리용
	UPROPERTY()
	TMap<FName, FTimerHandle> BuffDebuffTimers;

	// 위젯 항목 관리용
	UPROPERTY()
	TMap<FName, UBuffDebuffEntryWidget*> BuffDebuffEntries;

	void OnBuffDebuffExpired(FName ID);

	void Test_AddMultipleBuffs();
	
};
