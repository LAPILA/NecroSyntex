// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
