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
		return; // Monster�� nullptr�̸� �� �̻� �������� ����
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
					//// ���� Ÿ�̸� ���� (����������)
					//GetWorld()->GetTimerManager().SetTimer(RestoreSpeedHandle, [this, player, reducedSpeed]()
					//	{
					//		player->WalkSpeed += reducedSpeed;  // Ÿ�̸� ���� �� �̵� �ӵ� ����
					//	}, 3.0f, false);

					//// Ŭ���̾�Ʈ���� �̵� �ӵ� ������ �˸��� ��Ƽĳ��Ʈ ȣ��
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
			float DamageAmount = Monster->MonsterAD * 1.5; // �Ǵ� MonsterAD
			UGameplayStatics::ApplyDamage(Target, DamageAmount, Monster->GetController(), Monster, nullptr); //targetactor, targetdamage, causercontroller, causeractor, damagetype
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage!!!!!!!!!!"));
		}
	}
}

//void UMonsterSkillNotify::MulticastRestoreSpeed(AActor* Target, float speed)
//{
//	APlayerCharacter* player = Cast<APlayerCharacter>(Target);
//	if (player && !player->HasAuthority()) {
//		// Ŭ���̾�Ʈ������ �̵� �ӵ��� ����
//		player->WalkSpeed += speed;
//	}
//}

