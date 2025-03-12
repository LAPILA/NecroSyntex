#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "PlayerInformData.h"
#include "UObject/NoExportTypes.h"
#include "DopingParent.generated.h"

/**
 * Doping 아이템의 부모 클래스. Buff/DeBuff와 쿨타임 로직을 관리.
 */
UCLASS(Blueprintable)
class NECROSYNTEX_API UDopingParent : public UObject
{
	GENERATED_BODY()

public:
	UDopingParent();

	// 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DopingItemNum;

	// 쿨타임 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool Able; // 사용 가능 여부

	// Buff/DeBuff 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuffDuration; // 버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeBuffDuration; // 디버프 지속시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CheckBuff; // 버프 활성 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CheckDeBuff; // 디버프 활성 상태

	// 아이템 사용 함수
	UFUNCTION(BlueprintCallable, Category = "Doping")
	virtual void UseDopingItem(UPlayerInformData* PID);

	// Buff/DeBuff 처리 함수 (오버라이드 가능)
	UFUNCTION(BlueprintCallable, Category = "Doping")
	virtual void BuffOn(UPlayerInformData* PID);
	UFUNCTION(BlueprintCallable, Category = "Doping")
	virtual void BuffOff(UPlayerInformData* PID);
	UFUNCTION(BlueprintCallable, Category = "Doping")
	virtual void DeBuffOn(UPlayerInformData* PID);
	UFUNCTION(BlueprintCallable, Category = "Doping")
	virtual void DeBuffOff(UPlayerInformData* PID);

	// 내부 타이머 관리
	FTimerHandle BuffTimerHandle;
	FTimerHandle DeBuffTimerHandle;
	FTimerHandle CooldownTimerHandle;

	// 타이머 시작 함수
	void StartBuff(UPlayerInformData* PID);
	void StartDeBuff(UPlayerInformData* PID);
	void StartCooldown();

	// 타이머 종료 함수
	void EndBuff(UPlayerInformData* PID);
	void EndDeBuff(UPlayerInformData* PID);
	void EndCooldown();
};