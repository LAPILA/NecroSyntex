// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "AIController.h"
#include "NecroSyntex/Mission/DefenseTarget.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Mission/MissionComp.h"
#include "NecroSyntex/Monster/MonsterAnimInstance.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "M_Spawner.h"
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

// Sets default values
ABasicMonsterAI::ABasicMonsterAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
	ChaseSpeed = 0.0f;
	SlowChaseSpeed = 70.0f;
	SlowTime = 3.0f;
	//SkillAttackCoolTime = 15.0f;
	MonsterDistance = 50.0f;
	CanAttack = true;
	MeleeAttack = false;
	//CanSkill = true;
	hitCool = false;
	isAttackArea = false;
	isCanAttack = false;
	ensureAni = false;
}

// Called when the game starts or when spawned
void ABasicMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	DefaultChaseSpeed = ChaseSpeed;
}

// Called every frame
void ABasicMonsterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABasicMonsterAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicMonsterAI::UpdateWalkSpeed()
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = DefaultChaseSpeed;
	}
}

void ABasicMonsterAI::UpdateSpeed(float speed)
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = speed;
	}
}

//Weapon Damage
float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CanAttack = false;
	// 이미 죽은 상태면 처리하지 않음
	if (MonsterHP <= 0.0f) {
		return 0.0f;
	}

	// 데미지를 입힌 플레이어의 컨트롤러
	ANecroSyntexPlayerController* DPC = Cast<ANecroSyntexPlayerController>(EventInstigator);
	APlayerCharacter* DPA = Cast<APlayerCharacter>(DPC->GetPawn());

	if (!DPA) {
		//UE_LOG(LogTemp, Warning, TEXT(""));
		return 0.0f;
	}

	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);
	
	MonsterHP -= DamageAmount + DPA->DopingDamageBuff;
	
	if (!hitCool) {
		UAnimInstance* MonsterAnim = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnimInstance = Cast<UMonsterAnimInstance>(MonsterAnim);
		if (!MonsterAnimInstance->isScreamSkillTime) {
			hitCool = true;
			if (DamageAmount > 0) {
				PlayHitAnimation();
				DelayedAnimation(0.6f);
			}
		}
	}
	// 사망 처리
	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);

		MonsterAnim->DieTime = true;

		//MonsterStopMove();
		MoveStop_Implementation();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GetMesh())
		{
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		
		PlayDeathAnimation();
	}
	return DamageAmount;
}

//Doping Damage
void ABasicMonsterAI::TakeDopingDamage(float DopingDamageAmount)
{
	CanAttack = false;
	//UE_LOG(LogTemp, Warning, TEXT("TakeDopingDamage1"));

	if (MonsterHP <= 0.0f) {
		return;
	}

	//MonsterStopMove();

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();
	//while take damage monster damage animation cool time need?

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);


	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);

		MonsterAnim->DieTime = true;

		//MonsterStopMove();
		MoveStop_Implementation();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GetMesh())
		{
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		PlayDeathAnimation();
	}
	return;
}

void ABasicMonsterAI::MonsterStopMove()
{
	GetCharacterMovement()->MaxWalkSpeed = stopSpeed;
}

void ABasicMonsterAI::AttackCoolTime()
{
	CanAttack = true;
}

void ABasicMonsterAI::PlayHitAnimation()//약한 데미지인 경우 hit 애니메이션 재생
{
	if (HasAuthority()) {
		Multicast_PlayHitAnimation();
	}
}

void ABasicMonsterAI::Multicast_PlayHitAnimation_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Multicast_PlayHitAnimation_Implementation Call"));
	bIsHit = true;
	/*if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}*/
	if (ensureAni) {
		return;
	}

	bIsHit = true;
	OnRep_IsHit();
	ForceNetUpdate();
	FlushNetDormancy();
}

void ABasicMonsterAI::OnRep_IsHit()
{
	if (!bIsHit) {
		return;
	}

	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("OnRep Hit Call"));
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()//죽음 애니메이션 재생
{
	if (!HasAuthority()) {
		return;
	}
	bIsDead = true;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Server : PlayDeathAnimation Start"));
	//Multicast_PlayDeathAnimation();
	
	OnRep_IsDead();
	ForceNetUpdate();
	FlushNetDormancy();
}

void ABasicMonsterAI::Multicast_PlayDeathAnimation_Implementation()
{
	//none
}

void ABasicMonsterAI::OnRep_IsDead()
{
	if (!bIsDead || bAlreadyDead) {
		return;
	}
	
	// 죽음 몽타주 재생
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
		{
			if (auto* MonsterAnim = Cast<UMonsterAnimInstance>(Anim))
			{
				MonsterAnim->DieTime = true;
			}

			if (DeathReactionMontage)
			{
				bAlreadyDead = true;
				Anim->Montage_Play(DeathReactionMontage);
			}
		}
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->UnPossess();
	}

	DelayedFunction(2.2f);
}

void ABasicMonsterAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasicMonsterAI, ensureAni);
	DOREPLIFETIME_CONDITION_NOTIFY(ABasicMonsterAI, bIsDead, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ABasicMonsterAI, bIsStopMove, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ABasicMonsterAI, bIsHit, COND_None, REPNOTIFY_OnChanged);
}

void ABasicMonsterAI::OnRep_StopMove()
{
	if (!bIsStopMove) {
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("OnRep_StopMove"));
	GetCharacterMovement()->MaxWalkSpeed = stopSpeed;

	if (AAIController* AI = Cast<AAIController>(GetController())) {
		AI->StopMovement();
	}
}

void ABasicMonsterAI::MoveStop_Implementation()
{
	bIsStopMove = true;
	ensureAni = true;

	OnRep_StopMove();
	ForceNetUpdate();
	FlushNetDormancy();
}

void ABasicMonsterAI::PlayAttackAnimation()//공격 애니메이션 재생
{
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeathReaction Start"));
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)//일정 시간 동안 비동기적으로 함수가 실행되다가 destroy
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
}

void ABasicMonsterAI::DelayedAnimation(float DelayTime)
{
	GetWorld()->GetTimerManager().SetTimer(StopAnimationHandle, this, &ABasicMonsterAI::StopAnimation, DelayTime, false);
}

void ABasicMonsterAI::StopAnimation()
{
	hitCool = false;
	bIsHit = false;
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

void ABasicMonsterAI::MoveToPlayer()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ANecroSyntexGameState* GameState = Cast<ANecroSyntexGameState>(UGameplayStatics::GetGameState(GetWorld()));
	ADefenseTarget* DefenseTarget = nullptr;

	for (TObjectIterator<ADefenseTarget> It; It; ++It) {
		DefenseTarget = *It;
		if (DefenseTarget) {
			// DefenseTarget을 찾았으면
			break;
		}
	}

	if (Player) {
		AAIController* AIController = Cast<AAIController>(GetController());

		if (DefenseTarget && GameState) {
			if (AIController && GameState->CurrentMission == "Defense") {
				AIController->MoveToActor(DefenseTarget, 10.0f, true, true, true, 0, true);
				return;
			}
		}
		
		if (AIController && GameState->CurrentMission != "Defense") {
			if (MeleeAttack) {//비교적 근접 공격을 하는 경우.
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("move success~~~~~~~~``"));
				AIController->MoveToActor(Player, MonsterDistance, true, true, true, 0, true);
			}
			else {
				AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
			}
		}
	}
}

void ABasicMonsterAI::ServerAttackPlayer()
{
	AttackPlayer();
}

void ABasicMonsterAI::AttackOverlap(AActor* OtherActor)
{
	UAnimInstance* BaseAnim = GetMesh()->GetAnimInstance();

	UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(BaseAnim);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("cast success!!!!!"));
	if (MonsterAnim->AttackTiming) {
		APlayerCharacter* TargetPlayer = Cast<APlayerCharacter>(OtherActor);
		if (TargetPlayer) {
			TargetPlayer->ReceiveDamage(TargetPlayer, MonsterAD, nullptr, GetController(), this);
		}
		else {
			ADefenseTarget* DefenseObject = Cast<ADefenseTarget>(OtherActor);
			if (DefenseObject) {
				DefenseObject->TakedDamage(DefenseObject, MonsterAD);
			}
		}
	}
}

void ABasicMonsterAI::FuncScream()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("scream broadcast "));
	ScreamStart.Broadcast();
}

void ABasicMonsterAI::FindPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("scream call"));
	FuncScream();
}

void ABasicMonsterAI::StartScreamTime(float delayTime)
{
	GetWorld()->GetTimerManager().SetTimer(ScreamStartPoint, this, &ABasicMonsterAI::FuncScream, delayTime, false);
}

