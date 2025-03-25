// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGranade.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AProjectileGranade : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileGranade();
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
private:

	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;
};
