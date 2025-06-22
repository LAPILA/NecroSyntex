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
#include "NecroSyntex/Monster/MonsterAnimInstance.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "M_Spawner.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABasicMonsterAI::ABasicMonsterAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	valueStopAnimationSound = false;
	isAttackArea = false;
	isCanAttack = false;
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

//Weapon Damage
float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CanAttack = false;
	// �̹� ���� ���¸� ó������ ����
	if (MonsterHP <= 0.0f) {
		return 0.0f;
	}

	// �������� ���� �÷��̾��� ��Ʈ�ѷ�
	ANecroSyntexPlayerController* DPC = Cast<ANecroSyntexPlayerController>(EventInstigator);
	APlayerCharacter* DPA = Cast<APlayerCharacter>(DPC->GetPawn());

	if (!DPA) {
		UE_LOG(LogTemp, Warning, TEXT(""));
		return 0.0f;
	}

	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);
	//if (GEngine)
	//{
	//	FString DamageText = FString::Printf(TEXT("Taked : %f"), DamageAmount + DPA->DopingDamageBuff);
	//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DamageText);
	//}
	MonsterHP -= DamageAmount + DPA->DopingDamageBuff;
	
	if (!valueStopAnimationSound) {
		valueStopAnimationSound = true;
		if (DamageAmount > 0) {//Refactoring Need..
			//UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
			PlayHitAnimation();
			DelayedAnimationSound(0.6f);
		}
	}
	// ��� ó��
	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		AController* TempController = GetController();
		AAIController* AIController = Cast<AAIController>(TempController);

		MonsterAnim->DieTime = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GetMesh())
		{
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (AIController) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller UnPossess()"));
			AIController->UnPossess();  // AIController ����
		}

		PlayDeathAnimation();

		MonsterStopMove();

		DelayedFunction(2.2f); // ���� �ð� �� ���� �Ǵ� ������
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

	MonsterStopMove();

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);


	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		AController* TempController = GetController();
		AAIController* AIController = Cast<AAIController>(TempController);

		MonsterAnim->DieTime = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GetMesh())
		{
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (AIController) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller UnPossess()"));
			AIController->UnPossess();  // AIController ����
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller Nope"));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayDeath"));

		PlayDeathAnimation();

		MonsterStopMove();

		DelayedFunction(2.2f); // ���� �ð� �� ����
	}
	return;
}

void ABasicMonsterAI::MonsterStopMove()
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}
}

void ABasicMonsterAI::AttackCoolTime()
{
	CanAttack = true;
}

void ABasicMonsterAI::PlayHitAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HasAuthority()) {
		Multicast_PlayHitAnimation();
	}
}

void ABasicMonsterAI::Multicast_PlayHitAnimation_Implementation()
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayHitHighDamageAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(HitHighDamageReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()//���� �ִϸ��̼� ���
{
	if (DeathReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeathReaction Start"));
		GetMesh()->GetAnimInstance()->Montage_Play(DeathReactionMontage);
	}
}

void ABasicMonsterAI::PlayAttackAnimation()//���� �ִϸ��̼� ���
{
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeathReaction Start"));
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)//���� �ð� ���� �񵿱������� �Լ��� ����Ǵٰ� destroy
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
}

void ABasicMonsterAI::DelayedAnimationSound(float DelayTime)
{
	GetWorld()->GetTimerManager().SetTimer(SetAnimationSound, this, &ABasicMonsterAI::StopAnimationSound, DelayTime, false);
}

void ABasicMonsterAI::StopAnimationSound()
{
	valueStopAnimationSound = false;
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

//void ABasicMonsterAI::Attack_Player()//c++�� ���� �õ������� ������ �̹� �صּ� �н� playx4 ���ķ� ���� ����.
//{
//	//AttackPlayer();
//	/*if (AttackMontage) {
//		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//		if (AnimInstance) {
//			AnimInstance->Montage_Play(AttackMontage);
//			UE_LOG(LogTemp, Warning, TEXT("Playing Attack Montage"));
//		}
//	}
//	MoveToPlayer();*/
//}

void ABasicMonsterAI::MoveToPlayer()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ANecroSyntexGameState* GameState = Cast<ANecroSyntexGameState>(UGameplayStatics::GetGameState(GetWorld()));
	ADefenseTarget* DefenseTarget = nullptr;

	for (TObjectIterator<ADefenseTarget> It; It; ++It) {
		DefenseTarget = *It;
		if (DefenseTarget) {
			// DefenseTarget�� ã������
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
			if (MeleeAttack) {//���� ���� ������ �ϴ� ���.
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("move success~~~~~~~~``"));
				AIController->MoveToActor(Player, MonsterDistance, true, true, true, 0, true);
				//UE_LOG(LogTemp, Warning, TEXT("Moving to Player 0511"));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("moveto"));
				/*if(AIController->MoveToActor(Player, MonsterDistance, true, true, true, 0, true)) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("success~~~~~~~~``"));
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fail~~~~~~~~~~~~"));
				}*/
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("Moving to Player 0511 2"));
				AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("move to end~~~~~~~~~~~~~"));
			}
		}
	}
}

void ABasicMonsterAI::SpawnNiagaraEffect(FVector SpawnLocation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Spawn Niagara"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GasBombEffect, SpawnLocation);
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