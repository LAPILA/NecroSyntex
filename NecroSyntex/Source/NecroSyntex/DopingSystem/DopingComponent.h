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
#include "NecroSyntex/Character/PlayerCharacter.h"
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

	UPROPERTY()
	UDopingParent* OneKeyDoping; // 키보드 1번키 도핑
	UPROPERTY(ReplicatedUsing = OnRep_FirstDopingCode)
	int FirstDopingCode;
	UFUNCTION()
	void OnRep_FirstDopingCode();
	UPROPERTY(Replicated)
	bool OneKeyBool;

	UPROPERTY()
	UDopingParent* TwoKeyDoping; // 키보드 2번키 도핑
	UPROPERTY(ReplicatedUsing = OnRep_SecondDopingCode)
	int SecondDopingCode;
	UFUNCTION()
	void OnRep_SecondDopingCode();
	UPROPERTY(Replicated)
	bool TwoKeyBool;

	UFUNCTION()
	void InitDopingSkillSet();

	//패시브
	UPROPERTY(Replicated)
	float Passive_Duration = 5.0f;

	UPROPERTY(Replicated)
	bool passive_call;

	UFUNCTION()
	virtual void Passive_Start();
	
	UFUNCTION()
	virtual void Passive_End();

	
	UPROPERTY(ReplicatedUsing = OnRep_OneItemNum, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
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
	UPROPERTY(ReplicatedUsing = OnRep_OneAble, EditAnywhere, BlueprintReadWrite, Category = "FirstDopping")
	bool One_Able; // 사용 가능 & 불가능

	UFUNCTION()
	void OnRep_OneAble();

	UFUNCTION()
	void OnRep_OneItemNum();


	UPROPERTY(ReplicatedUsing = OnRep_TwoItemNum, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
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
	UPROPERTY(ReplicatedUsing = OnRep_TwoAble, EditAnywhere, BlueprintReadWrite, Category = "SecondDopping")
	bool Two_Able; // 사용 가능 & 불가능

	UFUNCTION()
	void OnRep_TwoAble();

	UFUNCTION()
	void OnRep_TwoItemNum();

	//데미지
	//임시 총 데미지
	UPROPERTY(Replicated)
	float GunDamage = 50.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Damage")
	//([총 데미지] * [캐릭터 공격력 배율])*도핑강화배율(무통증(10%), 마지막 불꽃(20%), 합연산)
	float TotalDamage;

	UFUNCTION(BlueprintCallable)
	void PressedFirstDopingKey();

	UFUNCTION(Server, Reliable)
	void ServerPressedFirstDopingKey();

	UFUNCTION(BlueprintCallable)
	void PressedSecondDopingKey();

	UFUNCTION(Server, Reliable)
	void ServerPressedSecondDopingKey();

	UFUNCTION()
	virtual void FirstDopingUse();

	UFUNCTION()
	virtual void SecondDopingUse();

	UFUNCTION()
	void FirstDopingCoolStart();
	
	UFUNCTION()
	void FirstDopingCoolEnd();


	UFUNCTION()
	void SecondDopingCoolStart();

	UFUNCTION()
	void SecondDopingCoolEnd();

	FTimerHandle FirstDopingCoolTimehandle;
	FTimerHandle SecondDopingCoolTimehandle;

	UFUNCTION()
	void SetDopingKey(UDopingParent*& DopingKey, int32 Num);


	UFUNCTION(Server, Reliable)
	void SetFirstDopingKey(int32 Num);

	UFUNCTION(Server, Reliable)
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
	bool DopingforAllyMode;

	UFUNCTION(BlueprintCallable)
	void DopingModeChange();

	UFUNCTION(Server, Reliable)
	void ServerDopingModeChange();

	UFUNCTION(Client, Reliable)
	void ClientPlayDopingEffect();


	// 3. 아군에게 도핑을 주는 함수
	UFUNCTION()
	virtual void FirstDopingForAlly();

	UFUNCTION()
	virtual void SecondDopingForAlly();



	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EndGameDopingFinish();

	UFUNCTION(BlueprintCallable)
	void SetDopingIconHUD();


};
