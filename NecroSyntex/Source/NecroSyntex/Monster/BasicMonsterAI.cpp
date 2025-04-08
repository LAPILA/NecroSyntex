// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
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
}

// Called when the game starts or when spawned
void ABasicMonsterAI::BeginPlay()
{
	Super::BeginPlay();

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
	// 이미 죽은 상태면 처리하지 않음
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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MonsterHP--"));
	}
	
	if (DamageAmount < 50) {//Refactoring Need..
		PlayHitAnimation();
	}
	else {
		PlayHitHighDamageAnimation();
	}

	// 디버그 메시지
	if (GEngine)
	{
		FString DamageMsg = FString::Printf(TEXT("Hit! Damage: %.1f | HP: %.1f"), DamageAmount, MonsterHP);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DamageMsg);
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
		//ChaseSpeed = 0.0f;
		UpdateWalkSpeed();

		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead!"));

		DelayedFunction(3.5f); // 일정 시간 후 제거 또는 리스폰
	}

	return DamageAmount;
}

//Doping Damage
void ABasicMonsterAI::TakeDopingDamage(float DopingDamageAmount)
{
	if (MonsterHP <= 0) {
		AController* AIController = GetController();

		if (AIController)
		{
			AIController->UnPossess();  // AIController 해제
		}

		//CanAttack = false;
		PlayDeathAnimation();
		//ChaseSpeed = 0.0f;
		UpdateWalkSpeed();

		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead! Cause Doping"));

		DelayedFunction(3.5f); // 일정 시간 후 제거 또는 리스폰
	}

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();
	DelayedFunction(3.0f);
}

void ABasicMonsterAI::PlayHitAnimation()
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayHitHighDamageAnimation()
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitHighDamageReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()
{
	
	if (DeathReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathReactionMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]() { PlayDeathAnimation();}), DelayTime, false);
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

void ABasicMonsterAI::Attack_Player()
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
			if (MeleeAttack) {
				AIController->MoveToActor(Player, 100.0f, true, true, true, 0, true);
			}
			else {
				AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
			}
		}
	}
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