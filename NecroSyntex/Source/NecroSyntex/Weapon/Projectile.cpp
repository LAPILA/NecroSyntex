#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	// 충돌 박스 초기화
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	// 투사체 이동 컴포넌트 초기화
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true; // 투사체가 이동 방향을 따라 회전
	ProjectileMovementComponent->bShouldBounce = false;          // 반사 비활성화
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;    // 초기 속도
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;            // 최대 속도

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}
	// 투사체의 방향과 속도를 동기화
	FVector LaunchDirection = GetActorForwardVector(); // Actor의 정면 방향
	if (!InitialDirection.IsNearlyZero())
	{
		LaunchDirection = InitialDirection.GetSafeNormal(); // 설정된 초기 방향 우선 사용
	}
	ProjectileMovementComponent->Velocity = LaunchDirection * InitialSpeed;

	// 디버그 로그 (방향과 속도 확인)
	UE_LOG(LogTemp, Warning, TEXT("Projectile Velocity: %s"), *ProjectileMovementComponent->Velocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile Forward Vector: %s"), *LaunchDirection.ToString());
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
