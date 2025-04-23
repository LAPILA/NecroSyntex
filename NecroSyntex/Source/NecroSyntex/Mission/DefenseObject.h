// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefenseObject.generated.h"

UCLASS()
class NECROSYNTEX_API ADefenseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADefenseObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	float Health;

	UPROPERTY(Replicated)
	bool Survive;

	UPROPERTY(Replicated)
	bool Active;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RegionName;

	UFUNCTION(Server, Reliable)
	void DefenseObjectActive();

	UFUNCTION(Server, Reliable)
	void DefenseObjectDestroy();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void TakedDamage(AActor* DamagedActor, float Damage);

};
