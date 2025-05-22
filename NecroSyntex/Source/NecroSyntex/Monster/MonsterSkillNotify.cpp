// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSkillNotify.h"
#include "BasicMonsterAI.h"
#include "Kismet/GameplayStatics.h"

void UMonsterSkillNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABasicMonsterAI* Monster = Cast<ABasicMonsterAI>(MeshComp->GetOwner());

	if (!Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster is nullptr"));
		return; // Monster가 nullptr이면 더 이상 진행하지 않음
	}

	for (AActor* Target : Monster->GetOverlappingPlayers()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify!!!"));
		float DamageAmount = Monster->MonsterAD * 1.5; // 또는 MonsterAD
		UGameplayStatics::ApplyDamage(Target, DamageAmount, Monster->GetController(), Monster, nullptr); //targetactor, targetdamage, causercontroller, causeractor, damagetype
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage!!!!!!!!!!"));
	}
}

