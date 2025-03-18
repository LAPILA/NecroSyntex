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

	void PlayHitAnimation();

	void PlayDeathAnimation();

	void DestroyMonster();

	FTimerHandle DeathDelayTimerHandle;
	//Timer Function
	void DelayedFunction(float DelayTime);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};