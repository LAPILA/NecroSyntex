// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABasicMonsterAI::ABasicMonsterAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MonsterHP = 100.0f;
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

void ABasicMonsterAI::UpdateWalkSpeed(float NewWalkSpeed) {
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
	}
}

float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (MonsterHP <= 0.0f) {
		return 0.0f; // 이미 죽었으면 데미지 무효
	}

	MonsterHP -= DamageAmount; // 체력 감소

	PlayHitAnimation();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit!!!"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Monster Hit! Remaining Health: %f"), MonsterHP);

	// 체력이 0 이하이면 몬스터 제거
	if (MonsterHP <= 0.0f)
	{
		PlayDeathAnimation();
		UE_LOG(LogTemp, Warning, TEXT("Monster is Dead!"));
		DelayedFunction(3.0f);
		//Destroy();
	}

	return DamageAmount;
}

void ABasicMonsterAI::PlayHitAnimation()
{
	// 애니메이션이 설정되어 있는 경우 실행
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()
{
	// 애니메이션이 설정되어 있는 경우 실행
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
	Destroy(); // AActor의 Destroy() 실행
}