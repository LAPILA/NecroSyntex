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

	UFUNCTION()
	void UpdateWalkSpeed(float NewWalkSpeed);

	/** 몬스터의 추격 속도 */
	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseSpeed = 600.0f;

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual float TakeDamage_Implementation(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathReactionMontage;

	// 애니메이션 실행 함수
	void PlayHitAnimation();

	void PlayDeathAnimation();

	void DestroyMonster();

	//Death 이후 delaytime이후에 destroy
	FTimerHandle DeathDelayTimerHandle;
	//Timer Function
	void DelayedFunction(float DelayTime);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
