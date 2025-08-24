// Fill out your copyright notice in the Description page of Project Settings.

#include "TenAxe_MonsterAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include <Net/UnrealNetwork.h>

ATenAxe_MonsterAI::ATenAxe_MonsterAI()
{

}

void ATenAxe_MonsterAI::UpdateMaxWalkSpeed(float inputSpeed)
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = inputSpeed;
	}
}

void ATenAxe_MonsterAI::PlayScreamAnimation()
{
	/*if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(ScreamMontage);
	}*/
	Server_MonsterScream_Implementation();
}

void ATenAxe_MonsterAI::OnRep_PlayScream()
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(ScreamMontage);
	}
}

void ATenAxe_MonsterAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ATenAxe_MonsterAI, bIsScream, COND_None, REPNOTIFY_OnChanged);
}

void ATenAxe_MonsterAI::Server_MonsterScream_Implementation()
{
	bIsScream = true;
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(ScreamMontage);
	}

	OnRep_PlayScream();
	ForceNetUpdate();
	FlushNetDormancy();
}



