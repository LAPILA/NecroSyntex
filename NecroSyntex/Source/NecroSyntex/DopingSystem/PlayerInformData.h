// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerInformData.generated.h"

/**
 *
 */
UCLASS()
class NECROSYNTEX_API UPlayerInformData : public UObject
{
	GENERATED_BODY()

public:

	UPlayerInformData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth; // 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth; // 현재 체력
	UPROPERTY(EditAnywhere)
	float AttackPointMag; // 공격력 배율
	UPROPERTY(EditAnywhere)
	float MoveSpeed; // 이동속도
	UPROPERTY(EditAnywhere)
	float RunningSpeed; // 달리기속도



	//float Redbound; // 반동

	UPROPERTY(EditAnywhere)
	float MLAtaackPoint; // 근접 공격력
	UPROPERTY(EditAnywhere)
	float Defense; // 방어력
	UPROPERTY(EditAnywhere)
	float Blurred; // 시야(화면 흐림도)
	UPROPERTY(EditAnywhere)
	float ROF; // 총 연사속도
	//float Item_UseRate; // 아이템 사용비율

	float BaseMaxHealth;
	float BaseCurrentHealth;
	float BaseAttackPointMag;
	float BaseMoveSpeed;
	float BaseRunningSpeed;
	//float BaseRebound;
	float BaseMLAttackPoint;
	float BaseDefense;
	float BaseBlurred;
	float BaseROF;



};
