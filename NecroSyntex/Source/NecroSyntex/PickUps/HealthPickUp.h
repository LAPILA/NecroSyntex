// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "HealthPickUp.generated.h"

/**
 * 
 */
UCLASS()
class NECROSYNTEX_API AHealthPickUp : public APickUp
{
	GENERATED_BODY()
public:
	AHealthPickUp();
	virtual void Destroyed() override;
protected:
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBoxIndex,
		bool bFromSweap,
		const FHitResult& SweepResult);
private:

	UPROPERTY(EditAnywhere)
	float HealAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 2.f;


};
