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
		return; // Monster�� nullptr�̸� �� �̻� �������� ����
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
				
				//�ٸ���ȭ ����
				Doping_LegForce(player);
				
				//������ ����
				Doping_PainLess(player);
				
				//������ �Ҳ� ����
				Doping_FinalEmber(player);
				
				//�ܴ��� ��� ����
				Doping_SolidFortress(player);

				//��ȣ�� ���� ����
				Doping_Paradox(player);

				//ȥ���� ���� ����
				Doping_CurseofChaos(player);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("nonononoeeeeee"));
			}
		}
	}
	else {
		for (AActor* Target : Monster->GetOverlappingPlayers()) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify!!!"));
			float DamageAmount = Monster->MonsterAD * 1.5f; // �Ǵ� MonsterAD
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
		//restorePlayer->GetCharacterMovement()->MaxWalkSpeed = originWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("speed restored complete!!!!"));
		targetPlayer.Remove(restorePlayer);
	}
	
}

void UMonsterSkillNotify::Doping_LegForce(APlayerCharacter* player)
{
	if (!player->UDC->LegEnforce->GetBuff() && !player->UDC->LegEnforce->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - LegNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - LegNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->LegEnforce->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - LegNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - LegNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->LegEnforce->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - LegNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - LegNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_PainLess(APlayerCharacter* player)
{
	if (!player->UDC->Painless->GetBuff() && !player->UDC->Painless->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - PainLessNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - PainLessNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->Painless->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - PainLessNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - PainLessNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->Painless->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - PainLessNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - PainLessNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_FinalEmber(APlayerCharacter* player)
{
	if (!player->UDC->FinalEmber->GetBuff() && !player->UDC->FinalEmber->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - FinalEmberNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - FinalEmberNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->FinalEmber->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - FinalEmberNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - FinalEmberNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->FinalEmber->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - FinalEmberNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - FinalEmberNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_Paradox(APlayerCharacter* player)
{
	if (!player->UDC->ParadoxofGuardianship->GetBuff() && !player->UDC->ParadoxofGuardianship->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - ParadoxNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - ParadoxNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->ParadoxofGuardianship->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - ParadoxNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - ParadoxNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->ParadoxofGuardianship->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - ParadoxNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - ParadoxNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_SolidFortress(APlayerCharacter* player)
{
	if (!player->UDC->SolidFortress->GetBuff() && !player->UDC->SolidFortress->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - SolidFortressNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - SolidFortressNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->SolidFortress->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - SolidFortressNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - SolidFortressNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->SolidFortress->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - SolidFortressNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - SolidFortressNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}

void UMonsterSkillNotify::Doping_CurseofChaos(APlayerCharacter* player)
{
	if (!player->UDC->CurseofChaos->GetBuff() && !player->UDC->CurseofChaos->GetDeBuff()) {
		//���� ���Ұ� + Ÿ�̸�
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("no buff debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - CurseofChaosNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - CurseofChaosNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		//player->GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->CurseofChaos->GetBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - CurseofChaosNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - CurseofChaosNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
	else if (player->UDC->CurseofChaos->GetDeBuff()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use debuff"));
		slowWalkSpeed = FMath::Max(0.0f, originWalkSpeed - CurseofChaosNumber);
		slowRunningSpeed = FMath::Max(0.0f, originRunningSpeed - CurseofChaosNumber);
		player->WalkSpeed = slowWalkSpeed;
		player->RunningSpeed = slowRunningSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
		player->GetWorldTimerManager().SetTimer(RestoreHandle, this, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
	}
}


