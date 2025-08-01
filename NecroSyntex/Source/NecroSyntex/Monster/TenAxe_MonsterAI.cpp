// Fill out your copyright notice in the Description page of Project Settings.

#include "TenAxe_MonsterAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

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
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(ScreamMontage);
	}
}



