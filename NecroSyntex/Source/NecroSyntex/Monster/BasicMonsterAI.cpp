// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
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
	//SkillAttackArea->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapBegin);

	//AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	//AttackPoint->SetupAttachment(RootComponent);
	//AttackPoint->SetCollisionProfileName(TEXT("Trigger"));
	//AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnAttackAreaOverlap);

	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
	ChaseSpeed = 500.0f;
	SlowChaseSpeed = 70.0f;
	SlowTime = 3.0f;
	SkillAttackCoolTime = 15.0f;
	CanAttack = true;
	MeleeAttack = false;
	CanSkill = true;
}

// Called when the game starts or when spawned
void ABasicMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	//SkillBoxComponent overlab event bind.
	if (SkillAttackArea) {
		SkillAttackArea->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapBegin);
		SkillAttackArea->OnComponentEndOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SkillAttackArea is nullptr"));
	}
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
	//CanAttack = true;
	if (GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("Chase Speed %f"), ChaseSpeed);
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

//Weapon Damage
float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CanAttack = false;
	// 이미 죽은 상태면 처리하지 않음
	if (MonsterHP <= 0.0f) {
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Already Dead"));
		}
		return 0.0f;
	}

	// 데미지를 입힌 플레이어의 컨트롤러
	ANecroSyntexPlayerController* DPC = Cast<ANecroSyntexPlayerController>(EventInstigator);
	APlayerCharacter* DPA = Cast<APlayerCharacter>(DPC->GetPawn());

	if (!DPA) {
		UE_LOG(LogTemp, Warning, TEXT("박두림 바보"));
		return 0.0f;
	}


	if (GetCharacterMovement())//스킬 사용 시 데미지를 받으면 이동속도가 업데이트되어 생기게 되고 이로 인해 슬라이딩하는 모션이 발생.
	{
		GetCharacterMovement()->MaxWalkSpeed = SlowChaseSpeed;
	}

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);

	MonsterHP -= DamageAmount + DPA->DopingDamageBuff;
	
	if (DamageAmount < 50) {//Refactoring Need..
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitAnimation();
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitHighDamageAnimation();
	}

	// 디버그 메시지
	if (GEngine)
	{
		FString DamageMsg = FString::Printf(TEXT("Hit! Damage: %.1f | HP: %.1f"), DamageAmount + DPA->DopingDamageBuff, MonsterHP);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DamageMsg);
	}

	// 사망 처리
	if (MonsterHP <= 0.0f)
	{
		AController* AIController = GetController();

		if (AIController)
		{
			AIController->UnPossess();  // AIController 해제
		}

		//CanAttack = false;
		PlayDeathAnimation();
		ChaseSpeed = 0.0f;
		UpdateWalkSpeed();

		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead!"));

		DelayedFunction(3.5f); // 일정 시간 후 제거 또는 리스폰
	}

	return DamageAmount;
}

//Doping Damage
void ABasicMonsterAI::TakeDopingDamage(float DopingDamageAmount)
{
	CanAttack = false;
	//UE_LOG(LogTemp, Warning, TEXT("TakeDopingDamage1"));

	if (MonsterHP <= 0.0f) {
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Death"));
		}
		return;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);


	if (MonsterHP <= 0.0f) {
		AController* AIController = GetController();

		if (AIController)
		{
			AIController->UnPossess();  // AIController 해제
		}

		PlayDeathAnimation();
		ChaseSpeed = 0.0f;
		UpdateWalkSpeed();
		//UE_LOG(LogTemp, Warning, TEXT("Monster is Dead! Cause Doping"));
		DelayedFunction(3.5f); // 일정 시간 후 제거 또는 리스폰
	}
	return;
}

void ABasicMonsterAI::AttackCoolTime()
{
	CanAttack = true;
}

void ABasicMonsterAI::PlayHitAnimation()//약한 데미지인 경우 hit 애니메이션 재생
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayHitHighDamageAnimation()//강한 데미지인 경우 hit 애니메이션 재생
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitHighDamageReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()//죽음 애니메이션 재생
{
	
	if (DeathReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathReactionMontage);
	}
}

void ABasicMonsterAI::PlaySkillAttackAnimation()//스킬 공격 애니메이션 재생
{
	//UE_LOG(LogTemp, Warning, TEXT("Playing SkillAttack Montage"));
	if (SkillAttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(SkillAttackMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)//일정 시간 동안 비동기적으로 함수가 실행되다가 destroy
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]() { PlayDeathAnimation();}), DelayTime, false);
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

void ABasicMonsterAI::Attack_Player()//c++로 구현 시도했지만 블프로 이미 해둬서 패스 playx4 이후로 삭제 예정.
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
			//UE_LOG(LogTemp, Warning, TEXT("Moving to Player"));
			if (MeleeAttack) {//비교적 근접 공격을 하는 경우.
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
			//UE_LOG(LogTemp, Warning, TEXT("Player entered skill area: %s"), *OtherActor->GetName());
		}
		if (CanSkill) {
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
			MonsterAnim->isSkillAttackTime = true;

			if (MonsterAnim->isSkillAttackTime) {
				CanAttack = false;
				//UE_LOG(LogTemp, Warning, TEXT("Skill!!!!!!!!!!!!"));
				ChaseSpeed = 0;//Monster Stop.
				UpdateWalkSpeed();
				UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
				SkillAttack();

				ChaseSpeed = 300;//Monster can move.
				GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
			}
		}
		else {
			//UE_LOG(LogTemp, Warning, TEXT("Return OnOverLap"));
			return;
		}
		CanSkill = false;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cool Time Start"));
		GetWorld()->GetTimerManager().SetTimer(MonsterSkillCoolTime, this, &ABasicMonsterAI::SkillCoolTime, SkillAttackCoolTime, false);
	}
}

void ABasicMonsterAI::OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//isSkillAttackTime = false;
	if (OverlappingPlayers.Contains(OtherActor))
	{
		OverlappingPlayers.Remove(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("Player left skill area: %s"), *OtherActor->GetName());
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Skill Cool Complete"));
	CanSkill = true;
	//GetWorld()->GetTimerManager().ClearTimer(MonsterSkillCoolTime);
}
