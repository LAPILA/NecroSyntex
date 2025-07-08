// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAttackAnimNotify.h"
#include "EliteMonsterAI.h"
#include "BasicMonsterAI.h"

void UMonsterAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABasicMonsterAI* Monster = Cast<ABasicMonsterAI>(MeshComp->GetOwner());
	
	if (Monster) {
		Monster->CanAttack = true;
		UE_LOG(LogTemp, Warning, TEXT("CanAttack True"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Attack Notify Error"));
	}
}