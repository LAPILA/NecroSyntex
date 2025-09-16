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


	//Monster Speed.
	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DefaultChaseSpeed;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SlowChaseSpeed;

	UPROPERTY(Replicated)
	float stopSpeed = 0.0f;


	//Timer
	FTimerHandle SpeedRestoreTimerHandle;

	FTimerHandle AttackRestoreTimerHandle;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SlowTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* HandMesh;

	//Monster Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterHP;

	//Monster attack damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterAD; 

	//���� �����ϴ� ������ ���.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool MeleeAttack;

	//���� �������� ���� ����.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool CanAttack;

	//�̼Ǹ��� ���� �̵� �� ��ǥ ������Ʈ���� �Ÿ� ������ ���� ����.
	UPROPERTY()
	float MonsterDistance;

	//�⺻ �̵��ӵ��� ������Ʈ
	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(); //float NewWalkSpeed in parameter

	//�Ű����������� �̵��ӵ� ������Ʈ
	UFUNCTION(BlueprintCallable)
	void UpdateSpeed(float speed);

	//���� ��Ÿ��
	UFUNCTION(BlueprintCallable)
	void AttackCoolTime();

	UFUNCTION(BlueprintCallable)
	void TakeDopingDamage(float DopingDamageAmount);

	UFUNCTION(BlueprintCallable)
	void MoveToPlayer();

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	virtual float TakeDamage_Implementation(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnWeaponHitEvent(const FHitResult& HitResult);

	//���� �̵��ӵ� 0���� ������Ʈ
	UFUNCTION()
	void MonsterStopMove();

	//Scream ��ε�ĳ��Ʈ
	UPROPERTY(BlueprintAssignable)
	FScreamStartEvent ScreamStart;

	//ScreamStart event broadcast.
	UFUNCTION()
	void FuncScream();

	//�÷��̾� �߰� �� FuncScream()ȣ���� ���� �Լ�. AC_Monster���� ���.
	UFUNCTION(BlueprintCallable)
	void FindPlayer();

	//��ȿ ��ų �� ��� ����� �ƴ� ���� �ð� ���Ŀ� ����ϵ��� �ϱ� ���� �Լ�.
	UFUNCTION(BlueprintCallable)
	void StartScreamTime(float delayTime);
	FTimerHandle ScreamStartPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//���� ������ �������� ���� ����.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	bool isAttackArea;

	//Player ĳ������ �� ���� �������� �˷��ִ� ����.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	bool isCanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY()
	bool hitCool;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool ensureAni;

	UPROPERTY()
	FTimerHandle StopAnimationHandle;

	UPROPERTY()
	FTimerHandle DeathDelayTimerHandle;

	//OnRep setting.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//<---Hit
	UFUNCTION()
	void PlayHitAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_IsHit)
	bool bIsHit = false;

	UFUNCTION()
	void OnRep_IsHit();
	//--->
	
	
	//<---Death
	UFUNCTION()
	void PlayDeathAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;

	UPROPERTY()
	bool bAlreadyDead = false;

	UFUNCTION()
	void OnRep_IsDead();
	//--->

	//After death, move stop
	UPROPERTY(ReplicatedUsing = OnRep_StopMove)
	bool bIsStopMove = false;

	UFUNCTION()
	void OnRep_StopMove();

	UFUNCTION(Server,Reliable)
	void MoveStop();

	UFUNCTION()
	void PlayAttackAnimation();

	UFUNCTION()
	void DestroyMonster();

	//Timer Function
	UFUNCTION()
	void DelayedFunction(float DelayTime);

	UFUNCTION()
	void DelayedAnimation(float DelayTime);

	UFUNCTION()
	void StopAnimation();

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