// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGranade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileGranade::AProjectileGranade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	//duream code edit collision setting.
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileGranade::BeginPlay()
{
	AActor::BeginPlay();

	if (GetOwner())
	{
		ProjectileMesh->IgnoreActorWhenMoving(GetOwner(), true);
	}

	SpawnTrailSystem();
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGranade::OnBounce);
	
	//CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AProjectileGranade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("bounce~~~~~"));
	//ProjectileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}

void AProjectileGranade::Destroyed()
{
	ExplodeDamage();
	Super::Destroyed();
}