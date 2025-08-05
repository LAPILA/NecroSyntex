// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSkillNotify.h"
#include "BasicMonsterAI.h"
#include "EliteMonsterAI.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
				if (player->HasAuthority()) {
					/*float reducedSpeed = player->WalkSpeed * 0.2f;
					player->WalkSpeed -= reducedSpeed;*/

					//FTimerHandle RestoreSpeedHandle;
					//// 복원 타이머 설정 (서버에서만)
					//GetWorld()->GetTimerManager().SetTimer(RestoreSpeedHandle, [this, player, reducedSpeed]()
					//	{
					//		player->WalkSpeed += reducedSpeed;  // 타이머 종료 후 이동 속도 복원
					//	}, 3.0f, false);

					//// 클라이언트에게 이동 속도 변경을 알리는 멀티캐스트 호출
					//MulticastRestoreSpeed(Target, reducedSpeed);
				}
				//GetWorld()->GetTimerManager().SetTimer(RestoreSpeedHandle, this, &AEliteMonsterAI::, 0.02f, false);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("nonononoeeeeee"));
			}
		}
	}
	else {
		for (AActor* Target : Monster->GetOverlappingPlayers()) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify!!!"));
			float DamageAmount = Monster->MonsterAD * 1.5; // 또는 MonsterAD
			UGameplayStatics::ApplyDamage(Target, DamageAmount, Monster->GetController(), Monster, nullptr); //targetactor, targetdamage, causercontroller, causeractor, damagetype
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage!!!!!!!!!!"));
		}
	}
}

//void UMonsterSkillNotify::MulticastRestoreSpeed(AActor* Target, float speed)
//{
//	APlayerCharacter* player = Cast<APlayerCharacter>(Target);
//	if (player && !player->HasAuthority()) {
//		// 클라이언트에서는 이동 속도를 복원
//		player->WalkSpeed += speed;
//	}
//}

