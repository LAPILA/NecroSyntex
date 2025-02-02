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

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
private:
	// �浹 ������Ʈ
	UPROPERTY(EditAnywhere, Category = "Projectile|Collision")
	class UBoxComponent* CollisionBox;

	// ����ü �̵� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Projectile|Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// �ʱ� �߻� ����
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	FVector InitialDirection = FVector::ForwardVector;

	// �ʱ� �ӵ�
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	float InitialSpeed = 3000.0f;

	// �ִ� �ӵ�
	UPROPERTY(EditAnywhere, Category = "Projectile|Settings")
	float MaxSpeed = 3000.0f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;
	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};
