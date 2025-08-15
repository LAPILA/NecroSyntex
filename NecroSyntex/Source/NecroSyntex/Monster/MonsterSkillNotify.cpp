// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSkillNotify.h"
#include "BasicMonsterAI.h"
#include "EliteMonsterAI.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NecroSyntex/DopingSystem/DPFinalEmber.h"
#include "NecroSyntex/DopingSystem/DPCurseofChaos.h"
#include "NecroSyntex/DopingSystem/DPLegEnforce.h"
#include "NecroSyntex/DopingSystem/DPPainless.h"
#include "NecroSyntex/DopingSystem/DPParadoxofGuardianship.h"
#include "NecroSyntex/DopingSystem/DPSolidFortress.h"
#include "NecroSyntex/DopingSystem/DopingComponent.h"
#include "TimerManager.h" 
#include "Kismet/GameplayStatics.h"

void UMonsterSkillNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AEliteMonsterAI* Monster = Cast<AEliteMonsterAI>(MeshComp->GetOwner());

	if (!Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster is nullptr"));
		return; // Monster가 nullptr이면 더 이상 진행하지 않음
	}

	if (isScreamSkill) {
		for (AActor* Target : Monster->GetScreamOverlappingPlayers()) {
			APlayerCharacter* player = Cast<APlayerCharacter>(Target);
			if (player) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("player"));
				//player->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
				originWalkSpeed = player->WalkSpeed;
				originRunningSpeed = player->RunningSpeed;
				targetPlayer.Add(player);
				
				if (!player->UDC->LegEnforce->GetBuff() && !player->UDC->LegEnforce->GetDeBuff()) {
					//평상시 감소값 + 타이머
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
					slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - 300.0f);
					slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - 300.0f);
					player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
					player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
				}
				else if (player->UDC->LegEnforce->GetBuff()) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
					slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - 300.0f);
					slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - 300.0f);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
					player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
				}
				else if (player->UDC->LegEnforce->GetDeBuff()) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
					slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - 300.0f);
					slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - 300.0f);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
					player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
				}
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("nonononoeeeeee"));
			}
		}
	}
	else {
		for (AActor* Target : Monster->GetOverlappingPlayers()) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify!!!"));
			float DamageAmount = Monster->MonsterAD * 1.5f; // 또는 MonsterAD
			UGameplayStatics::ApplyDamage(Target, DamageAmount, Monster->GetController(), Monster, nullptr); //targetactor, targetdamage, causercontroller, causeractor, damagetype
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage!!!!!!!!!!"));
		}
	}
}

void UMonsterSkillNotify::RestoredSpeed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("speed restored start!!!!"));
	for (auto restorePlayer : targetPlayer) {
		if (!restorePlayer.IsValid()) {
			targetPlayer.Remove(restorePlayer);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("failed restored speed!!!!"));
			return;
		}
		restorePlayer->WalkSpeed = originWalkSpeed;
		restorePlayer->RunningSpeed = originRunningSpeed;
		restorePlayer->GetCharacterMovement()->MaxWalkSpeed = originWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("speed restored complete!!!!"));
		targetPlayer.Remove(restorePlayer);
	}
	
}


