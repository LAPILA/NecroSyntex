// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DopingParent.h"
#include "DPLegEnforce.h"
#include "DPReducePain.h"
#include "DPSupremeStrength.h"
#include "DPForcedHealing.h"
#include "DPFinalEmber.h"
#include "DPBurningFurnace.h"
#include "DPSolidFortress.h"
#include "DPPainless.h"
#include "PlayerInformData.h"
#include "Components/ActorComponent.h"
#include "DopingComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NECROSYNTEX_API UDopingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDopingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UDopingParent* OneKeyDoping; // 키보드 1번키 도핑
	bool OneKeyBool;

	UPROPERTY()
	UDopingParent* TwoKeyDoping; // 키보드 2번키 도핑
	bool TwoKeyBool;

	UPROPERTY()
	UPlayerInformData* PID;
	UPlayerInformData* PIDCheck;

	//패시브
	UPROPERTY()
	float Passive_Duration;

	UFUNCTION()
	virtual void Passive_Start();
	UFUNCTION()
	virtual void Passive_End();



	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	int One_DopingItemNum; // 도핑 아이템 개숫
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_BuffDuration; // 버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_CheckBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_CheckDeBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_Able; // 사용 가능 & 불가능


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	int Two_DopingItemNum; // 도핑 아이템 개숫
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_BuffDuration; // 버프지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckDeBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_Able; // 사용 가능 & 불가능


	// 캐릭터 스텟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Redbound; // 반동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MLAtaackPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Blurred;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float ROF;
	//float ItemUseRate; // 아이템 사용비율

	//데미지
	//임시 총 데미지
	UPROPERTY()
	float GunDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	//([총 데미지] * [캐릭터 공격력 배율])*도핑강화배율(무통증(10%), 마지막 불꽃(20%), 합연산)
	float TotalDamage;


	// 도핑 설정 및 사용
	UFUNCTION(BlueprintCallable)
	void FirstDopingUse();

	UFUNCTION(BlueprintCallable)
	void SecondDopingUse();

	void SetDopingKey(UDopingParent*& DopingKey, int32 Num);


	UFUNCTION(BlueprintCallable)
	void SetFirstDopingKey(int32 Num);

	UFUNCTION(BlueprintCallable)
	void SetSecondDopingKey(int32 Num);

	UDopingComponent* GetDopingComponent();

	FTimerHandle PassiveTimerHandle;


	//TArray<UDopingParent*> ActiveDopingItems;


	// 아군에게 도핑 주기
	// 1, 일단 캐릭터 마다 도핑 오브젝트를 가지고는 있다.

	UPROPERTY()
	UDPLegEnforce LegEnforce;

	UPROPERTY()
	UDPReducePain ReducePain;

	UPROPERTY()
	UDPSupremeStrength SupremeStrength;

	UPROPERTY()
	UDPForcedHealing ForcedHealing;

	UPROPERTY()




};
