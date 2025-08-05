// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NecroSyntex/Interfaces/InteractWithCrossHairsInterface.h"
#include "BasicMonsterAI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScreamStartEvent);

UCLASS()
class NECROSYNTEX_API ABasicMonsterAI : public ACharacter, public IInteractWithCrossHairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicMonsterAI();

	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DefaultChaseSpeed;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* GasBombEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool CanAttack;

	UPROPERTY()
	float MonsterDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FName KeyName;

	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(); //float NewWalkSpeed in parameter

	UFUNCTION(BlueprintCallable)
	void AttackCoolTime();

	//UFUNCTION(BlueprintCallable)
	//void Attack_Player();

	UFUNCTION(BlueprintCallable)
	void TakeDopingDamage(float DopingDamageAmount);

	UFUNCTION(BlueprintCallable)
	void MoveToPlayer();

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	virtual float TakeDamage_Implementation(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnWeaponHitEvent(const FHitResult& HitResult);

	UFUNCTION()
	void MonsterStopMove();

	UPROPERTY(BlueprintAssignable)
	FScreamStartEvent ScreamStart;

	//ScreamStart event broadcast.
	UFUNCTION()
	void FuncScream();

	//플레이어 발견 시 FuncScream()호출을 위한 함수.
	UFUNCTION(BlueprintCallable)
	void FindPlayer();

	//포효 스킬 시 즉시 사용이 아닌 일정 시간 이후에 사용하도록 하기 위한 함수.
	UFUNCTION(BlueprintCallable)
	void StartScreamTime(float delayTime);

	FTimerHandle ScreamStartPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	bool isAttackArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	bool isCanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitHighDamageReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;*/

	UPROPERTY()
	bool valueStopAnimationSound;

	UPROPERTY()
	FTimerHandle SetAnimationSound;

	UPROPERTY()
	FTimerHandle DeathDelayTimerHandle;

	UFUNCTION()
	void PlayHitAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitAnimation();

	UFUNCTION()
	void PlayHitHighDamageAnimation();

	UFUNCTION()
	void PlayDeathAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathAnimation();

	UFUNCTION()
	void PlayAttackAnimation();

	UFUNCTION()
	void DestroyMonster();

	//Timer Function
	UFUNCTION()
	void DelayedFunction(float DelayTime);

	UFUNCTION()
	void DelayedAnimationSound(float DelayTime);

	UFUNCTION()
	void StopAnimationSound();

	UFUNCTION(BlueprintImplementableEvent)
	void AttackPlayer();

	UFUNCTION(BlueprintCallable)
	void ServerAttackPlayer();

	UFUNCTION(BlueprintCallable)
	void AttackOverlap(AActor* OtherActor);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};