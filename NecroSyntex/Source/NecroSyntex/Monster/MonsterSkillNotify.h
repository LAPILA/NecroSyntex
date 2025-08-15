// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MonsterSkillNotify.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API UMonsterSkillNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere)
	bool isScreamSkill = false;

	/*UPROPERTY(EditAnywhere)
	float decreaseSpeed;*/

	UPROPERTY()
	float reducedSpeed = 0;

	//UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	//void MulticastRestoreSpeed(AActor* Target, float speed);
};
