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
#include "DPParadoxofGuardianship.h"
#include "DPHallucinationShield.h"
#include "DPHPconversion.h"
#include "DPCurseofChaos.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Replicated)
	UDopingParent* OneKeyDoping; // 키보드 1번키 도핑
	UPROPERTY(Replicated)
	int FirstDopingCode;
	UPROPERTY(Replicated)
	bool OneKeyBool;

	UPROPERTY(Replicated)
	UDopingParent* TwoKeyDoping; // 키보드 2번키 도핑
	UPROPERTY(Replicated)
	int SecondDopingCode;
	UPROPERTY(Replicated)
	bool TwoKeyBool;

	UPROPERTY(Replicated, BlueprintReadOnly)
	UPlayerInformData* PID;
	UPROPERTY(Replicated)
	UPlayerInformData* PIDCheck;

	//패시브
	UPROPERTY(Replicated)
	float Passive_Duration;

	UFUNCTION()
	virtual void Passive_Start();
	
	UFUNCTION()
	virtual void Passive_End();



	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	int One_DopingItemNum; // 도핑 아이템 개숫
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_BuffDuration; // 버프 지속시간
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	float One_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_CheckBuff;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_CheckDeBuff;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_Able; // 사용 가능 & 불가능


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	int Two_DopingItemNum; // 도핑 아이템 개숫
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DopingCoolTime; // 아이템 사용 쿨타임
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_BuffDuration; // 버프지속시간
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	float Two_DeBuffDuration; // 디버프 지속시간
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckBuff;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_CheckDeBuff;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_Able; // 사용 가능 & 불가능


	// 캐릭터 스텟
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MaxHealth;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float CurrentHealth;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MaxShield;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float CurrentShield;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MoveSpeed;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float RunningSpeed;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Rebound; // 반동
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float MLAtaackPoint;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Defense;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float Blurred;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerDopingInform")
	float ROF;
	//float ItemUseRate; // 아이템 사용비율

	//데미지
	//임시 총 데미지
	UPROPERTY(Replicated)
	float GunDamage = 50.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Damage")
	//([총 데미지] * [캐릭터 공격력 배율])*도핑강화배율(무통증(10%), 마지막 불꽃(20%), 합연산)
	float TotalDamage;


	// 도핑 설정 및 사용
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void FirstDopingUse();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void SecondDopingUse();

	UFUNCTION()
	void SetDopingKey(UDopingParent*& DopingKey, int32 Num);


	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetFirstDopingKey(int32 Num);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetSecondDopingKey(int32 Num);

	UDopingComponent* GetDopingComponent();

	FTimerHandle PassiveTimerHandle;


	//TArray<UDopingParent*> ActiveDopingItems;


	// 아군에게 도핑 주기
	// 1, 일단 캐릭터 마다 도핑 오브젝트를 가지고는 있다.

	UPROPERTY(Replicated)
	UDPLegEnforce* LegEnforce;

	UPROPERTY(Replicated)
	UDPReducePain* ReducePain;

	UPROPERTY(Replicated)
	UDPSupremeStrength* SupremeStrength;

	UPROPERTY(Replicated)
	UDPForcedHealing* ForcedHealing;

	UPROPERTY(Replicated)
	UDPFinalEmber* FinalEmber;

	UPROPERTY(Replicated)
	UDPBurningFurnace* BurningFurnace;

	UPROPERTY(Replicated)
	UDPSolidFortress* SolidFortress;

	UPROPERTY(Replicated)
	UDPPainless* Painless;

	UPROPERTY(Replicated)
	UDPParadoxofGuardianship* ParadoxofGuardianship;

	UPROPERTY(Replicated)
	UDPHallucinationShield* HallucinationShield;

	UPROPERTY(Replicated)
	UDPHPconversion* HPconversion;

	UPROPERTY(Replicated)
	UDPCurseofChaos* CurseofChaos;

	// 1-1. 아군에게 도핑주는 모드 전환
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool DopingMode;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void DopingModeChange();

	// 2. 라인트레이스로 아군을 찾는 함수

	// 3. 아군에게 도핑을 주는 함수
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void FirstDopingForAlly();


	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void SecondDopingForAlly();



};
