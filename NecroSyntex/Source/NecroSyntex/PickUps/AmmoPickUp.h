// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "NecroSyntex/Weapon/WeaponTypes.h"
#include "AmmoPickUp.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AAmmoPickUp : public APickUp
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBoxIndex,
		bool bFromSweap,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

};
