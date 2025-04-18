// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "NecroSyntex\NecroSyntaxComponents\LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

namespace
{
	static const TSet<FName> BodyBones =
	{
		TEXT("pelvis"), TEXT("spine_01"), TEXT("spine_02"), TEXT("spine_03"),
		TEXT("clavicle_l"), TEXT("clavicle_r"),
		TEXT("upperarm_l"), TEXT("upperarm_r"),
		TEXT("thigh_l"),   TEXT("thigh_r")
	};

	FORCEINLINE bool IsHeadBone(const FName& Bone) { return Bone == TEXT("head"); }
	FORCEINLINE bool IsBodyBone(const FName& Bone) { return BodyBones.Contains(Bone); }
}

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APlayerCharacter* Shooter = Cast<APlayerCharacter>(GetOwner());
	if (!Shooter) { Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit); return; }

	ANecroSyntexPlayerController* PC =
		Cast<ANecroSyntexPlayerController>(Shooter->Controller);
	if (!PC) { Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit); return; }

	auto CalcDamage = [&](const FName& Bone) -> float
		{
			if (IsHeadBone(Bone)) return HeadShotDamage;
			else if (IsBodyBone(Bone)) return Damage;
			else                       return SubDamage;
		};
	const float DamageToCause = CalcDamage(Hit.BoneName);

	if (Shooter->HasAuthority() && !bUseServerSideRewind)
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageToCause,
			PC, this, UDamageType::StaticClass());
		Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		return;
	}

	if (APlayerCharacter* Victim = Cast<APlayerCharacter>(OtherActor))
	{
		if (bUseServerSideRewind && Shooter->IsLocallyControlled() &&
			Shooter->GetLagCompensation())
		{
			Shooter->GetLagCompensation()->ProjectileServerScoreRequest(
				Victim,
				TraceStart,
				InitialVelocity,
				PC->GetServerTime() - PC->SingleTripTime);
		}
	}

	if (ABasicMonsterAI* Monster = Cast<ABasicMonsterAI>(OtherActor))
	{
		if (!Shooter->HasAuthority() && Shooter->IsLocallyControlled())
		{
			if (AWeapon* Wep = Shooter->GetEquippedWeapon())
			{
				Wep->Server_ApplyMonsterDamage(Monster, DamageToCause, PC);
			}
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}
