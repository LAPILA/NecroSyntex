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

	UFUNCTION()
	void RestoredSpeed();

	UPROPERTY(EditAnywhere)
	bool isScreamSkill = false;

	UPROPERTY()
	float originWalkSpeed;

	UPROPERTY()
	float originRunningSpeed;

	UPROPERTY()
	float slowWalkSpeed;

	UPROPERTY()
	float slowRunningSpeed;

	FTimerHandle RestoreHandle;

	TArray<TWeakObjectPtr<class APlayerCharacter>> targetPlayer;
};
