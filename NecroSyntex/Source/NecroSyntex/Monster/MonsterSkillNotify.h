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

	UFUNCTION()
	void Doping_LegForce(APlayerCharacter* player);

	UFUNCTION()
	void Doping_PainLess(APlayerCharacter* player);

	UFUNCTION()
	void Doping_FinalEmber(APlayerCharacter* player);

	UFUNCTION()
	void Doping_Paradox(APlayerCharacter* player);

	UFUNCTION()
	void Doping_SolidFortress(APlayerCharacter* player);

	UFUNCTION()
	void Doping_CurseofChaos(APlayerCharacter* player);

	//UFUNCTION()
	//void CallScreamSkill();

	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_CallScreamSkill();

	//UPROPERTY(ReplicatedUsing = OnRep_IsScream)
	//bool bIsScream = false;

	//UFUNCTION()
	//void OnRep_IsScream();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float legNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float painLessNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float finalEmberNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float paradoxNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float solidFortressNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float curseofChaosNumber = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doping Figure")
	float normalNumber = 300.0f;

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
