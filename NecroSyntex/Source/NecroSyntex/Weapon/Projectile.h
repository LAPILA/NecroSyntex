#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class NECROSYNTEX_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	// 충돌 컴포넌트
	UPROPERTY(EditAnywhere, Category = "Projectile|Collision")
	class UBoxComponent* CollisionBox;

	// 투사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Projectile|Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// 초기 발사 방향
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	FVector InitialDirection = FVector::ForwardVector;

	// 초기 속도
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	float InitialSpeed = 3000.0f;

	// 최대 속도
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	float MaxSpeed = 3000.0f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;
	class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};
