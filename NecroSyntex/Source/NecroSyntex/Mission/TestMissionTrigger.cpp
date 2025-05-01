// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMissionTrigger.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "Net/UnrealNetwork.h"

ATestMissionTrigger::ATestMissionTrigger()
{
	MissionDuration = 50.0f;
	MissionRegion = "Survival";
	MissionName = "Survival";
}

void ATestMissionTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestMissionTrigger, OnTrigger);
	DOREPLIFETIME(ATestMissionTrigger, OnTriggerTimer);
}

void ATestMissionTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!HasAuthority()) return;


	if (!OnTrigger) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerInTrigger++;

		ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
		if (GS && !GS->OngoingMission)
		{

			if (GS->TotalPlayer == PlayerInTrigger)
			{
				if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
				{
					//������ �̼� ����
					GM->MissionManager->MissionSet(MissionName, MissionRegion, MissionDuration);

					//�̼� ���� ī���� �ٿ� ���� (���߿� ���� ������ Ÿ�̸� ��� �� �ʱ�ȭ)
					GM->MissionManager->MissionCountdownStart();

					GetWorld()->GetTimerManager().SetTimer(
						TriggerTimer,
						[this]() { TriggerDestroy(); },
						GM->MissionManager->count,
						false
					);

					OnTriggerTimer = true;

				}
			}
		}
	}
}

void ATestMissionTrigger::NotifyActorEndOverlap(AActor* OtherActor) {
	if (!HasAuthority()) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerInTrigger--;
		ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			if (GS->TotalPlayer > PlayerInTrigger)
			{
				if (GS->MissionCountDownBool == true) {
					if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
					{
						GM->MissionManager->MissionCountdownCancel();
						GetWorld()->GetTimerManager().ClearTimer(TriggerTimer);
						OnTriggerTimer = false;
					}
				}
			}
		}
	}
}


void ATestMissionTrigger::TriggerDestroy()
{
	OnTrigger = false;
}