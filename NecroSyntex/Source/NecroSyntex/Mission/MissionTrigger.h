// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionTrigger.generated.h"

UCLASS()
class NECROSYNTEX_API AMissionTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionTrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* MissionTriggerBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnBoxTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY()
	USceneComponent* RootSceneComponent;

	UPROPERTY()
	int32 PlayerInTrigger = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MissionDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionRegion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission", meta = (GetOptions = "GetMissionNameOptions"))
	FName MissionName;

	UFUNCTION()
	TArray<FName> GetMissionNameOptions() const
	{
		return {
			FName("Survival"),
			FName("Defense"),
			FName("Rescue"),
			FName("Boss")
		};
	}

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	bool OnTrigger = true;

	UPROPERTY(Replicated)
	bool OnTriggerTimer = false;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MissionTriggerActivate();

	FTimerHandle TriggerTimer;

	UFUNCTION()
	void TriggerDestroy();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastLog();

	/*UFUNCTION(NetMulticast, Reliable)
	void DestroyMulticast();*/

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerMakeNoise();

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerDestroyCall();
};
