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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* ScreamSkillArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ScreamMontage;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateMoveSpeed(float inputSpeed);

	UFUNCTION()
	void PlaySkillAttackAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillAttackAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayScreamSkillAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_IsScreamSkill)
	bool bIsScreamSkill = false;

	UFUNCTION()
	void OnRep_IsScreamSkill();

	UFUNCTION()
	void SkillCoolTime(); //attack skill cooltime.

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

	UFUNCTION()
	void OnScreamSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnScreamSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TArray<AActor*> ScreamOverlappingPlayers;

	UFUNCTION()
	TArray<AActor*>& GetScreamOverlappingPlayers();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillAttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SkillAttackMontage;

	UPROPERTY()
	bool CanSkill;

	UFUNCTION()
	void CallAttackSkill();

	UPROPERTY(ReplicatedUsing = OnRep_IsSkillAttack)
	bool bIsSkillAttack = false;

	UFUNCTION()
	void OnRep_IsSkillAttack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void AttackSkillStart(float delayTime);

	FTimerHandle skillDelayTime;
};
