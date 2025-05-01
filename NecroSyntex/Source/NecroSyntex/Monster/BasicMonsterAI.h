// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "BasicMonsterAI.generated.h"

UCLASS()
class NECROSYNTEX_API ABasicMonsterAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicMonsterAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* SkillAttackArea;

	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SlowChaseSpeed;

	FTimerHandle SpeedRestoreTimerHandle;

	FTimerHandle AttackRestoreTimerHandle;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SlowTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* HandMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterAD; //attack damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool MeleeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundBase* AttackSound;

	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(); //float NewWalkSpeed in parameter

	UFUNCTION(BlueprintCallable)
	void AttackCoolTime();

	UFUNCTION(BlueprintCallable)
	void Attack_Player();

	UFUNCTION(BlueprintCallable)
	void TakeDopingDamage(float DopingDamageAmount);

	UFUNCTION(BlueprintCallable)
	void MoveToPlayer();

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	virtual float TakeDamage_Implementation(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnWeaponHitEvent(const FHitResult& HitResult);

	UPROPERTY()
	TArray<AActor*> OverlappingPlayers;

	TArray<AActor*>& GetOverlappingPlayers() { return OverlappingPlayers; }

	UPROPERTY()
	bool CanSkill;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitHighDamageReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SkillAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool CanAttack;

	//UFUNCTION(BlueprintCallable, Category = "AI")
	//void SkillAttackPrepare();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SkillAttack();

	UFUNCTION()
	void OnSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PlayHitAnimation();

	void PlayHitHighDamageAnimation();

	void PlayDeathAnimation();

	void DestroyMonster();

	void PlaySkillAttackAnimation();

	FTimerHandle MonsterSkillCoolTime;

	void SkillCoolTime();

	FTimerHandle DeathDelayTimerHandle;
	//Timer Function
	void DelayedFunction(float DelayTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};