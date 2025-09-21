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
				TWeakObjectPtr<APlayerCharacter>    WPlayer = player;
				TWeakObjectPtr<UMonsterSkillNotify> WThis = this;
				FTimerHandle Tmp;
				if (!IsValid(player)) {
					return;
				}
				if (!IsValid(player->UDC->LegEnforce)) {
					player->GetWorldTimerManager().SetTimer(
						Tmp,
						FTimerDelegate::CreateLambda([WThis, WPlayer]()
							{
								// 0.2초 뒤 재확인
								if (!WThis.IsValid() || !WPlayer.IsValid())
									return;

								APlayerCharacter* P = WPlayer.Get();
								if (!IsValid(P->UDC) || !IsValid(P->UDC->LegEnforce))
									return;

								WThis->Doping_LegForce(P);
							}),
						0.1f,
						false
					);
					continue;
				}
				if (!IsValid(player->UDC->Painless)) {
					return;
				}
				if (!IsValid(player->UDC->FinalEmber)) {
					return;
				}
				if (!IsValid(player->UDC->SolidFortress)) {
					return;
				}
				if (!IsValid(player->UDC->ParadoxofGuardianship)) {
					return;
				}
				if (!IsValid(player->UDC->CurseofChaos)) {
					return;
				}

				//다리강화 도핑
				if (player->UDC->LegEnforce->GetBuff() || player->UDC->LegEnforce->GetDeBuff()) {
					Doping_LegForce(player);
				}
				else if (player->UDC->Painless->GetBuff() || player->UDC->LegEnforce->GetDeBuff()) {
					//무통증 도핑
					Doping_PainLess(player);
				}
				else if (player->UDC->FinalEmber->GetBuff() || player->UDC->FinalEmber->GetDeBuff()) {
					//마지막 불꽃 도핑
					Doping_FinalEmber(player);
				}
				else if (player->UDC->SolidFortress->GetBuff() || player->UDC->SolidFortress->GetDeBuff()) {
					//단단한 요새 도핑
					Doping_SolidFortress(player);
				}
				else if (player->UDC->ParadoxofGuardianship->GetBuff() || player->UDC->ParadoxofGuardianship->GetDeBuff()) {
					//수호의 역설 도핑
					Doping_Paradox(player);
				}
				else if (player->UDC->CurseofChaos->GetBuff() || player->UDC->CurseofChaos->GetDeBuff()) {
					//혼돈의 저주 도핑
					Doping_CurseofChaos(player);
				}
				else {//status is not buff and debuff.
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("normal status"));
					slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - 300.0f);//로 수치 조정.
					slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - 300.0f);
					player->WalkSpeed = slowWalkSpeed;
					player->RunningSpeed = slowRunningSpeed;
					player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
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

// restore lag exist! need fix work.
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

void UMonsterSkillNotify::Doping_LegForce(APlayerCharacter* player)
{
	if (!player->HasAuthority()) {
		return;
	}
	if (!player->UDC->LegEnforce->GetBuff() && !player->UDC->LegEnforce->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Leg no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - 300.0f);//LegNumber로 수치 조정.
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - 300.0f);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->LegEnforce->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - legNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - legNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->LegEnforce->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - legNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - legNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_PainLess(APlayerCharacter* player)
{
	if (!player->UDC->Painless->GetBuff() && !player->UDC->Painless->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - painLessNumber);
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - painLessNumber);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		////player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->Painless->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - painLessNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - painLessNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->Painless->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - painLessNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - painLessNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_FinalEmber(APlayerCharacter* player)
{
	if (!player->UDC->FinalEmber->GetBuff() && !player->UDC->FinalEmber->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - finalEmberNumber);
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - finalEmberNumber);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		////player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->FinalEmber->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - finalEmberNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - finalEmberNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->FinalEmber->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - finalEmberNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - finalEmberNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_Paradox(APlayerCharacter* player)
{
	if (!player->UDC->ParadoxofGuardianship->GetBuff() && !player->UDC->ParadoxofGuardianship->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - paradoxNumber);
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - paradoxNumber);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		////player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->ParadoxofGuardianship->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - paradoxNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - paradoxNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->ParadoxofGuardianship->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - paradoxNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - paradoxNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_SolidFortress(APlayerCharacter* player)
{
	if (!player->UDC->SolidFortress->GetBuff() && !player->UDC->SolidFortress->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - solidFortressNumber);
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - solidFortressNumber);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		////player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->SolidFortress->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - solidFortressNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - solidFortressNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->SolidFortress->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - solidFortressNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - solidFortressNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_CurseofChaos(APlayerCharacter* player)
{
	if (!player->UDC->CurseofChaos->GetBuff() && !player->UDC->CurseofChaos->GetDeBuff()) {
		//평상시 감소값 + 타이머
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		//slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - curseofChaosNumber);
		//slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - curseofChaosNumber);
		//player->WalkSpeed = slowWalkSpeed;
		//player->RunningSpeed = slowRunningSpeed;
		////player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		//player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->CurseofChaos->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - curseofChaosNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - curseofChaosNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->CurseofChaos->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - curseofChaosNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - curseofChaosNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}


