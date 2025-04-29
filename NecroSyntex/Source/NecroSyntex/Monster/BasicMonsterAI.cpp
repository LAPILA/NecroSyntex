// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NecroSyntex/Monster/MonsterAnimInstance.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "M_Spawner.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABasicMonsterAI::ABasicMonsterAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	SkillAttackArea->SetGenerateOverlapEvents(true);

	//AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	//AttackPoint->SetupAttachment(RootComponent);
	//AttackPoint->SetCollisionProfileName(TEXT("Trigger"));
	//AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnAttackAreaOverlap);

	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
	ChaseSpeed = 500.0f;
	SlowChaseSpeed = 200.0f;
	SlowTime = 3.0f;
	CanAttack = true;
	MeleeAttack = false;
	CanSkill = true;
}

// Called when the game starts or when spawned
void ABasicMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	//SkillBoxComponent overlab event bind.
	SkillAttackArea->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapBegin);
	SkillAttackArea->OnComponentEndOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapEnd);
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

//Weapon Damage
float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// �̹� ���� ���¸� ó������ ����
	if (MonsterHP <= 0.0f) {
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Already Dead"));
		}
		return 0.0f;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SlowChaseSpeed;
	}

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	//UpdateWalkSpeed();// one seconds later call function. delayedfunction will set target function UpdataeWalkSpeed();

	MonsterHP -= DamageAmount;
	
	if (DamageAmount < 50) {//Refactoring Need..
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitAnimation();
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitHighDamageAnimation();
	}

	// ����� �޽���
	if (GEngine)
	{
		FString DamageMsg = FString::Printf(TEXT("Hit! Damage: %.1f | HP: %.1f"), DamageAmount, MonsterHP);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DamageMsg);
	}

	// ��� ó��
	if (MonsterHP <= 0.0f)
	{
		AController* AIController = GetController();

		if (AIController)
		{
			AIController->UnPossess();  // AIController ����
		}

		//CanAttack = false;
		PlayDeathAnimation();
		//ChaseSpeed = 0.0f;
		UpdateWalkSpeed();

		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead!"));

		DelayedFunction(3.5f); // ���� �ð� �� ���� �Ǵ� ������
	}

	return DamageAmount;
}

//Doping Damage
void ABasicMonsterAI::TakeDopingDamage(float DopingDamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("TakeDopingDamage"));

	if (MonsterHP <= 0.0f) {
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Death"));
		}
		return;
	}

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();

	if (MonsterHP <= 0.0f) {
		AController* AIController = GetController();

		if (AIController)
		{
			AIController->UnPossess();  // AIController ����
		}

		PlayDeathAnimation();
		UpdateWalkSpeed();
		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead! Cause Doping"));
		DelayedFunction(3.5f); // ���� �ð� �� ���� �Ǵ� ������
	}
	return;
}

void ABasicMonsterAI::PlayHitAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayHitHighDamageAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitHighDamageReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()//���� �ִϸ��̼� ���
{
	
	if (DeathReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathReactionMontage);
	}
}

void ABasicMonsterAI::PlaySkillAttackAnimation()//��ų ���� �ִϸ��̼� ���
{
	if (SkillAttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(SkillAttackMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)//���� �ð� ���� �񵿱������� �Լ��� ����Ǵٰ� destroy
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]() { PlayDeathAnimation();}), DelayTime, false);
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

void ABasicMonsterAI::Attack_Player()//c++�� ���� �õ������� ������ �̹� �صּ� �н� playx4 ���ķ� ���� ����.
{
	UE_LOG(LogTemp, Warning, TEXT("Monster Attack Triggered"));

	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage);
			UE_LOG(LogTemp, Warning, TEXT("Playing Attack Montage"));
		}
	}

	MoveToPlayer();
}

void ABasicMonsterAI::MoveToPlayer()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving to Player"));
			if (MeleeAttack) {//���� ���� ������ �ϴ� ���.
				AIController->MoveToActor(Player, 100.0f, true, true, true, 0, true);
			}
			else {
				AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
			}
		}
	}
}

void ABasicMonsterAI::OnSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//isSkillAttackTime = true;
	if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player"))
	{
		if (!OverlappingPlayers.Contains(OtherActor))
		{
			OverlappingPlayers.Add(OtherActor);
			UE_LOG(LogTemp, Warning, TEXT("Player entered skill area: %s"), *OtherActor->GetName());
		}
		if (CanSkill) {
			//CanSkill = false;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
			MonsterAnim->isSkillAttackTime = true;

			if (MonsterAnim->isSkillAttackTime) {
				UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
				SkillAttack();
				ChaseSpeed = 0;//Monster Stop.
				UpdateWalkSpeed();

				ChaseSpeed = 500;//Monster can move.
				GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cool Time Start"));
		//GetWorld()->GetTimerManager().SetTimer(MonsterSkillCoolTime, this, &ABasicMonsterAI::SkillCoolTime, 5.0f, false);
	}
}

void ABasicMonsterAI::OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//isSkillAttackTime = false;
	if (OverlappingPlayers.Contains(OtherActor))
	{
		OverlappingPlayers.Remove(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Player left skill area: %s"), *OtherActor->GetName());
	}
}

void ABasicMonsterAI::SkillAttack()
{
	PlaySkillAttackAnimation();
	//apply damage check code..
	//damage apply code..
}

void ABasicMonsterAI::SkillCoolTime()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Skill Cool Complete"));
	CanSkill = true;
	GetWorld()->GetTimerManager().ClearTimer(MonsterSkillCoolTime);
}

//void ABasicMonsterAI::OnAttackAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != this)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Monster Attack Overlapped with %s"), *OtherActor->GetName());
//
//		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//		if (!AnimInstance)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("No AnimInstance found"));
//			return;
//		}
//
//		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
//		if (!MonsterAnim)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to MonsterAnimInstance"));
//			return;
//		}
//
//		if (MonsterAnim->AttackTiming)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Monster is Attacking"));
//
//			APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
//			if (Player)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("Monster Attacked Player!"));
//			}
//		}
//	}
//}