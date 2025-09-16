// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeleteMonster.generated.h"

UCLASS()
class NECROSYNTEX_API ADeleteMonster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeleteMonster();

	UPROPERTY()
	TArray<AActor*> FoundMonsters;

	UFUNCTION(BlueprintCallable)
	void DeleteMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
