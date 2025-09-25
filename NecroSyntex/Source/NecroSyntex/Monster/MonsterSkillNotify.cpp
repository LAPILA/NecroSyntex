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
		return;
	}

	if (isScreamSkill) {
		for (AActor* Target : Monster->GetScreamOverlappingPlayers()) {
			APlayerCharacter* player = Cast<APlayerCharacter>(Target);
			if (player) {

				// 안전한 실행을 위해 약한 포인터(Weak Pointer)로 캡처합니다.
				TWeakObjectPtr<UMonsterSkillNotify> WeakThis = this;
				TWeakObjectPtr<APlayerCharacter> WeakPlayer = player;
				FTimerHandle DelayTimerHandle;

				// 월드의 타이머 관리자를 통해 0.1초 뒤에 람다 함수를 실행하도록 예약합니다.
				GetWorld()->GetTimerManager().SetTimer(
					DelayTimerHandle,
					[WeakThis, WeakPlayer]() // 람다 함수 시작
					{
						// 0.1초 뒤에 오브젝트가 유효한지 먼저 확인합니다.
						if (!WeakThis.IsValid() || !WeakPlayer.IsValid())
						{
							return;
						}

						// 실제 포인터를 가져옵니다.
						UMonsterSkillNotify* StrongThis = WeakThis.Get();
						APlayerCharacter* StrongPlayer = WeakPlayer.Get();

						// -----------------------------------------------------
						// ▼ 기존의 모든 도핑 관련 로직을 이곳으로 이동시킵니다.▼
						// -----------------------------------------------------
						StrongThis->originWalkSpeed = StrongPlayer->WalkSpeed;
						StrongThis->originRunningSpeed = StrongPlayer->RunningSpeed;
						StrongThis->targetPlayer.Add(StrongPlayer);

						// ... (기존 UDC 유효성 검사 로직)
						if (!IsValid(StrongPlayer->UDC) || !IsValid(StrongPlayer->UDC->LegEnforce)) {
							// 이 부분은 필요에 따라 유지하거나 조정할 수 있습니다.
							// 0.1초 딜레이 후에도 유효하지 않을 경우를 대비한 코드입니다.
							return;
						}

						// 다리강화 도핑
						if (StrongPlayer->UDC->LegEnforce->GetBuff() || StrongPlayer->UDC->LegEnforce->GetDeBuff()) {
							StrongThis->Doping_LegForce(StrongPlayer);
						}
						else if (StrongPlayer->UDC->Painless->GetBuff() || StrongPlayer->UDC->Painless->GetDeBuff()) {
							// 무통증 도핑
							StrongThis->Doping_PainLess(StrongPlayer);
						}
						else if (StrongPlayer->UDC->FinalEmber->GetBuff() || StrongPlayer->UDC->FinalEmber->GetDeBuff()) {
							// 마지막 불꽃 도핑
							StrongThis->Doping_FinalEmber(StrongPlayer);
						}
						else if (StrongPlayer->UDC->SolidFortress->GetBuff() || StrongPlayer->UDC->SolidFortress->GetDeBuff()) {
							// 단단한 요새 도핑
							StrongThis->Doping_SolidFortress(StrongPlayer);
						}
						else if (StrongPlayer->UDC->ParadoxofGuardianship->GetBuff() || StrongPlayer->UDC->ParadoxofGuardianship->GetDeBuff()) {
							// 수호의 역설 도핑
							StrongThis->Doping_Paradox(StrongPlayer);
						}
						else if (StrongPlayer->UDC->CurseofChaos->GetBuff() || StrongPlayer->UDC->CurseofChaos->GetDeBuff()) {
							// 혼돈의 저주 도핑
							StrongThis->Doping_CurseofChaos(StrongPlayer);
						}
						else { // status is not buff and debuff.
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("normal status"));
							StrongThis->slowWalkSpeed = FMath::Max(0.0f, StrongThis->originWalkSpeed - StrongThis->normalNumber); // 변수 사용
							StrongThis->slowRunningSpeed = FMath::Max(0.0f, StrongThis->originRunningSpeed - StrongThis->normalNumber); // 변수 사용
							StrongPlayer->WalkSpeed = StrongThis->slowWalkSpeed;
							StrongPlayer->RunningSpeed = StrongThis->slowRunningSpeed;
							StrongPlayer->GetCharacterMovement()->MaxWalkSpeed = StrongThis->slowWalkSpeed;
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timer start"));
							StrongPlayer->GetWorldTimerManager().SetTimer(StrongThis->RestoreHandle, StrongThis, &UMonsterSkillNotify::RestoredSpeed, 5.0f, false);
						}

					},
					0.1f,  // 지연 시간 (초)
					false  // 반복하지 않음
				);
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

//void UMonsterSkillNotify::CallScreamSkill()
//{
//	
//}
//
//void UMonsterSkillNotify::Multicast_CallScreamSkill()
//{
//
//}
//
//void UMonsterSkillNotify::OnRep_IsScream()
//{
//
//}


