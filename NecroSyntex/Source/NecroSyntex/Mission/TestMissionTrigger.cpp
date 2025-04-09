// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMissionTrigger.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"

ATestMissionTrigger::ATestMissionTrigger()
{
	MissionDuration = 5.0f;
	MissionRegion = "Survival";
}

void ATestMissionTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!HasAuthority()) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		UCapsuleComponent* PCCapsule = PC->GetCapsuleComponent();
		if(PCCapsule)

		if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->MissionManager->StartSurvivalMission(MissionRegion, MissionDuration);
		}
	}
}

void ATestMissionTrigger::PlayerTriggerOverlap(AActor* OtherActor)
{
	if (!HasAuthority()) return;

	if (Cast<APlayerCharacter>(OtherActor))
	{
		if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->MissionManager->StartSurvivalMission(MissionRegion, MissionDuration);
		}
	}
}