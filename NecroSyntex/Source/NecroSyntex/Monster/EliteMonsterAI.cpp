// Fill out your copyright notice in the Description page of Project Settings.

#include "EliteMonsterAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.h"
#include "NecroSyntex/Monster/MonsterAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

AEliteMonsterAI::AEliteMonsterAI()
{
	//PrimaryActorTick.bCanEverTick = false;

	SkillAttackArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SkillArea"));
	SkillAttackArea->SetupAttachment(RootComponent);
	SkillAttackArea->SetRelativeLocation(FVector(168.f, 0.f, 0.f));
	SkillAttackArea->SetBoxExtent(FVector(100.f, 50.f, 50.f));
	SkillAttackArea->SetCollisionProfileName(TEXT("Trigger"));
	SkillAttackArea->SetGenerateOverlapEvents(true);

	SkillAttackArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkillAttackArea->SetCollisionObjectType(ECC_WorldDynamic);
	SkillAttackArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkillAttackArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//SkillAttackArea->SetGenerateOverlapEvents(true);
	
	ScreamSkillArea = CreateDefaultSubobject<USphereComponent>(TEXT("ScreamArea"));
	ScreamSkillArea->SetupAttachment(RootComponent);
	ScreamSkillArea->SetCollisionProfileName(TEXT("ScreamTrigger"));
	ScreamSkillArea->SetGenerateOverlapEvents(true);
	ScreamSkillArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScreamSkillArea->SetCollisionObjectType(ECC_WorldDynamic);
	ScreamSkillArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	ScreamSkillArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
	ChaseSpeed = 0.0f;
	SlowChaseSpeed = 70.0f;
	SlowTime = 3.0f;
	SkillAttackCoolTime = 15.0f;
	MonsterDistance = 50.0f;
	CanAttack = true;
	MeleeAttack = false;
	CanSkill = true;
	valueStopAnimationSound = false;
	isAttackArea = false;
	isCanAttack = false;
}

void AEliteMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI start"));
	DefaultChaseSpeed = ChaseSpeed;

	if (SkillAttackArea) {
		SkillAttackArea->OnComponentBeginOverlap.AddDynamic(this, &AEliteMonsterAI::OnSkillAreaOverlapBegin);
		SkillAttackArea->OnComponentEndOverlap.AddDynamic(this, &AEliteMonsterAI::OnSkillAreaOverlapEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SkillAttackArea is nullptr"));
	}

	if (ScreamSkillArea) {
		ScreamSkillArea->OnComponentBeginOverlap.AddDynamic(this, &AEliteMonsterAI::OnScreamSkillAreaOverlapBegin);
		ScreamSkillArea->OnComponentEndOverlap.AddDynamic(this, &AEliteMonsterAI::OnScreamSkillAreaOverlapEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ScreamSkillArea is nullptr"));
	}

}

void AEliteMonsterAI::UpdateMoveSpeed(float inputSpeed)
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = inputSpeed;
	}
}

void AEliteMonsterAI::PlaySkillAttackAnimation()//스킬 공격 애니메이션 재생
{
	if (HasAuthority()) {
		Multicast_PlaySkillAttackAnimation();
	}
}

void AEliteMonsterAI::Multicast_PlaySkillAttackAnimation_Implementation()
{
	if (SkillAttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(SkillAttackMontage);
	}
}

void AEliteMonsterAI::OnSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//isSkillAttackTime = true;
	if (MonsterHP <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Skill"));
		return;
	}

	if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player")) {
		if (!OverlappingPlayers.Contains(OtherActor)) {
			OverlappingPlayers.Add(OtherActor);
		}
		UAnimInstance* BaseAnim = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* monsterAnim = Cast<UMonsterAnimInstance>(BaseAnim);
		if (!monsterAnim->isScreamSkillTime) {
			AttackSkillStart(1.0f);
		}
	}
}

void AEliteMonsterAI::OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappingPlayers.Contains(OtherActor)) {
		OverlappingPlayers.Remove(OtherActor);
	}
}

void AEliteMonsterAI::SkillAttack()
{
	PlaySkillAttackAnimation();
}

void AEliteMonsterAI::SkillCoolTime()
{
	CanSkill = true;
}

TArray<AActor*>& AEliteMonsterAI::GetOverlappingPlayers()
{
	return OverlappingPlayers;
}

void AEliteMonsterAI::OnScreamSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap!!!!!!!!!!!"));
	if (MonsterHP <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Skill"));
		return;
	}

	if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player")) {
		if (!ScreamOverlappingPlayers.Contains(OtherActor)) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("inin Overlap!!!!!!!!!!!"));
			ScreamOverlappingPlayers.Add(OtherActor);
		}
	}
}

void AEliteMonsterAI::OnScreamSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OverlapEND!!!!!!!!!!!"));
	if (ScreamOverlappingPlayers.Contains(OtherActor)) {
		ScreamOverlappingPlayers.Remove(OtherActor);
	}
}

TArray<AActor*>& AEliteMonsterAI::GetScreamOverlappingPlayers()
{
	return ScreamOverlappingPlayers;
}

void AEliteMonsterAI::CallAttackSkill()
{
	if (CanSkill) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		MonsterAnim->isSkillAttackTime = true;

		if (MonsterAnim->isSkillAttackTime) {
			CanAttack = false;

			MonsterStopMove();

			PlaySkillAttackAnimation();

			GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &AEliteMonsterAI::UpdateWalkSpeed, 2.3f, false);
			GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &AEliteMonsterAI::AttackCoolTime, 2.0f, false);
		}
	}
	else {
		return;
	}
	CanSkill = false;
	GetWorld()->GetTimerManager().SetTimer(MonsterSkillCoolTime, this, &AEliteMonsterAI::SkillCoolTime, SkillAttackCoolTime, false);
}

void AEliteMonsterAI::AttackSkillStart(float delayTime)
{
	GetWorld()->GetTimerManager().SetTimer(skillDelayTime, this, &AEliteMonsterAI::CallAttackSkill, delayTime, false);
}


