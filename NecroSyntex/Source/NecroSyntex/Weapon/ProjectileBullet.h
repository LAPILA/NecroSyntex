// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "NecroSyntex\Monster\BasicMonsterAI.h"
#include "NecroSyntex/Weapon/Weapon.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileBullet();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere)
	float TDamage;

};
