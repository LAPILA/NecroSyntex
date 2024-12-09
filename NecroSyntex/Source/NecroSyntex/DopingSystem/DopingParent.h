// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "PlayerInformData.h"
#include "UObject/NoExportTypes.h"
#include "DopingParent.generated.h"



/**
 *
 */
UCLASS()
class NECROSYNTEX_API UDopingParent : public UObject
{
	GENERATED_BODY()

public:
	UDopingParent();

	//UPlayerInformData* PID;

	UPROPERTY(EditAnywhere)
	int DopingItemNum; // 도핑 아이템 갯수

	UPROPERTY(EditAnywhere)
	float DopingCoolTime; // 아이템 사용 쿨타임

	UPROPERTY(EditAnywhere)
	float CurrentCoolTime; // 현재 쿨타임

	UPROPERTY(EditAnywhere)
	bool Able; // 사용 가능 & 불가능

	UPROPERTY(EditAnywhere)
	float BuffDuration;  // 버프 지속시간

	UPROPERTY(EditAnywhere)
	float BuffRemainDuration;  // 버프 남은 지속시간

	UPROPERTY(EditAnywhere)
	float DeBuffDuration; // 디버프 지속시간

	UPROPERTY(EditAnywhere)
	float DeBuffRemainDuration; // 디버프 남은 지속시간

	UPROPERTY(EditAnywhere)
	bool CheckBuff;

	UPROPERTY(EditAnywhere)
	bool CheckDeBuff;

	float XDeltaTime;

	UFUNCTION(BlueprintCallable)
	virtual void UseDopingItem(UPlayerInformData* PID);

	UFUNCTION(BlueprintCallable)
	virtual void BuffOn(UPlayerInformData* PID);

	UFUNCTION(BlueprintCallable)
	virtual void DeBuffOn(UPlayerInformData* PID);

	UFUNCTION(BlueprintCallable)
	virtual void BuffOff(UPlayerInformData* PID);

	UFUNCTION(BlueprintCallable)
	virtual void DeBuffOff(UPlayerInformData* PID);

	//void GetDeltaTime(float Deltatime);


	void StartCooldown();

private:

	FTimerHandle CooldownTimerHandle;


};
