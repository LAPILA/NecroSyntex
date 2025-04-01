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

	AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetCollisionProfileName(TEXT("Trigger"));
	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnAttackAreaOverlap);

	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
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

void ABasicMonsterAI::UpdateWalkSpeed(float NewWalkSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
	}
}

float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (MonsterHP <= 0.0f) {
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TEXT("Death")));
		}
		return 0.0f; 
	}

	MonsterHP -= DamageAmount; 

	PlayHitAnimation();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit!!!"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Monster Hit! Remaining Health: %f"), MonsterHP);

	
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
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
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
	UE_LOG(LogTemp, Warning, TEXT("DestroyMonster() called"));

	if (OnMonsterDestroyed.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("Broadcasting OnMonsterDestroyed event"));
		
		OnMonsterDestroyed.Broadcast();  // 이벤트가 바인딩 되었는지 확인 후 브로드캐스트
	}
	//OnMonsterDestroyed.Broadcast();
	//Spawner->ReduceMonster();
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

			AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
		}
	}
}

void ABasicMonsterAI::OnAttackAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Attack Overlapped with %s"), *OtherActor->GetName());

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("No AnimInstance found"));
			return;
		}

		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		if (!MonsterAnim)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to MonsterAnimInstance"));
			return;
		}

		if (MonsterAnim->AttackTiming)
		{
			UE_LOG(LogTemp, Warning, TEXT("Monster is Attacking"));

			APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
			if (Player)
			{
				UE_LOG(LogTemp, Warning, TEXT("Monster Attacked Player!"));
			}
		}
	}
}