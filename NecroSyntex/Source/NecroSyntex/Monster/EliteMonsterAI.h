// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicMonsterAI.h"
#include "EliteMonsterAI.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AEliteMonsterAI : public ABasicMonsterAI
{
	GENERATED_BODY()
	
public:
	AEliteMonsterAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* SkillAttackArea;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateMoveSpeed(float inputSpeed);

	UFUNCTION()
	void PlaySkillAttackAnimation();

	UFUNCTION()
	void SkillCoolTime();

	UPROPERTY()
	FTimerHandle MonsterSkillCoolTime;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SkillAttack();

	UFUNCTION()
	void OnSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TArray<AActor*> OverlappingPlayers;

	UFUNCTION()
	TArray<AActor*>& GetOverlappingPlayers();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillAttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SkillAttackMontage;

	UPROPERTY()
	bool CanSkill;
};
