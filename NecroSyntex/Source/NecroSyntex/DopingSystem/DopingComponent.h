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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReduceCooldown(float DeltaTime);
	void ReduceBuffDuration(float DeltaTime);
	void ReduceDeBuffDuration(float DeltaTime);

	//UDPLegEnforce* LE;
	//UDPReducePain* RP;
	//UDPSupremeStrength* SS;
	//UDPForcedHealing* FH;
	//UDPFinalEmber* FE;
	//UDPBurningFurnace* BF;
	//UDPSolidFortress* SF;
	//UDPPainless* PL;

	UPlayerInformData* PID;

	UDopingParent* OneKeyDoping; // 키보드 1번키 도핑
	bool OneKeyBool;
	UDopingParent* TwoKeyDoping; // 키보드 2번키 도핑
	bool TwoKeyBool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	int One_DopingItemNum; // 도핑 아이템 개숫
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_CurrentCoolTime; // 현재 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_BuffDuration; // 버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_BuffRemainDuration; // 버프 남은지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DeBuffRemainDuration; // 디버프 남은 지속시간
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
	float Two_CurrentCoolTime; // 현재 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_BuffDuration; // 버프지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_BuffRemainDuration; // 버프 남은지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DeBuffRemainDuration; // 디버프 남은 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckDeBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_Able; // 사용 가능 & 불가능


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float AttackPointMag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float RunningSpeed;
	//float Redbound; // 반동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MLAtaackPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Blurred;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float ROF;
	//float ItemUseRate; // 아이템 사용비율

	UFUNCTION(BlueprintCallable)
	void FirstDopingUse();

	UFUNCTION(BlueprintCallable)
	void SecondDopingUse();


	UFUNCTION(BlueprintCallable)
	void SetFirstDopingKey(int32 Num);

	UFUNCTION(BlueprintCallable)
	void SetSecondDopingKey(int32 Num);

	//TArray<UDopingParent*> ActiveDopingItems;




};
