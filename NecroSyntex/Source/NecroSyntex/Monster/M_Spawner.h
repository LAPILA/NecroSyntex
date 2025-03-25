// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "M_Spawner.generated.h"

UCLASS()
class NECROSYNTEX_API AM_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AM_Spawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	class UBoxComponent* SpawnPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
