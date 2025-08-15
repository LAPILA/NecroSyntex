// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles\ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "NecroSyntex\NecroSyntaxComponents\LagCompensationComponent.h"
#include "NecroSyntex\Monster\BasicMonsterAI.h"

#include "DrawDebugHelpers.h"

namespace
{
	static const TSet<FName> SCBodyBones =
	{
		TEXT("pelvis"), TEXT("spine_01"), TEXT("spine_02"), TEXT("spine_03"),
		TEXT("clavicle_l"), TEXT("clavicle_r"),
		TEXT("upperarm_l"), TEXT("upperarm_r"),
		TEXT("thigh_l"),   TEXT("thigh_r")
	};

	FORCEINLINE bool IsSCHeadBone(const FName& Bone) { return Bone == TEXT("head"); }
	FORCEINLINE bool IsSCBodyBone(const FName& Bone) { return SCBodyBones.Contains(Bone); }
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* InstController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
	if (!MuzzleSocket) return;

	//duream code add. playfiresound();
	PlayFireSound();

	/* ------------------------------------------------------------------ */
	/* 1) Trace                                                       */
	/* ------------------------------------------------------------------ */
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	const FVector     TraceStart = SocketTransform.GetLocation();

	FHitResult FireHit;
	WeaponTraceHit(TraceStart, HitTarget, FireHit);

	/* ------------------------------------------------------------------ */
	/* 2) Head / Body / Sub)                                 */
	/* ------------------------------------------------------------------ */
	auto CalcDamageForBone = [&](const FName& Bone) -> float
		{
			if (IsSCHeadBone(Bone)) return HeadShotDamage;
			else if (IsSCBodyBone(Bone)) return Damage;
			else                      return SubDamage;
		};

	const float DamageToCause = CalcDamageForBone(FireHit.BoneName);

	/* ------------------------------------------------------------------ */
	/* 3) Monster                             */
	/* ------------------------------------------------------------------ */
	if (ABasicMonsterAI* Monster = Cast<ABasicMonsterAI>(FireHit.GetActor()))
	{
		if (OwnerPawn->HasAuthority())
		{
			UGameplayStatics::ApplyDamage(Monster, DamageToCause, InstController, this, UDamageType::StaticClass());
		}
		else
		{
			Server_ApplyMonsterDamage(Monster, DamageToCause, InstController);
		}
	}

	/* ------------------------------------------------------------------ */
	/* 4) PlayerCharacter                */
	/* ------------------------------------------------------------------ */
	if (APlayerCharacter* Victim = Cast<APlayerCharacter>(FireHit.GetActor()))
	{
		if (OwnerPawn->HasAuthority() && (!bUseServerSideRewind || OwnerPawn->IsLocallyControlled()))
		{
			UGameplayStatics::ApplyDamage(Victim, DamageToCause, InstController, this, UDamageType::StaticClass());
		}
		else if (!OwnerPawn->HasAuthority() && bUseServerSideRewind)
		{
			APlayerCharacter* Shooter = Cast<APlayerCharacter>(OwnerPawn);
			auto* PC = Cast<ANecroSyntexPlayerController>(InstController);
			if (Shooter && PC && Shooter->GetLagCompensation())
			{
				Shooter->GetLagCompensation()->ServerScoreRequest(
					Victim,
					TraceStart,
					HitTarget,
					PC->GetServerTime() - PC->SingleTripTime
				);
			}
		}
	}

	/* ------------------------------------------------------------------ */
	/* 5) VFX / SFX                                                      */
	/* ------------------------------------------------------------------ */
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), ImpactParticles,
			FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
	}
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

		FCollisionQueryParams QueryParams;
		QueryParams.bReturnPhysicalMaterial = false;
		QueryParams.AddIgnoredActor(this);
		if (AActor* MyOwner = GetOwner())
		{
			QueryParams.AddIgnoredActor(MyOwner);
		}

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Pawn,
			QueryParams
		);

		if (OutHit.bBlockingHit && OutHit.GetActor())
		{
			APlayerCharacter* HitCharacter = Cast<APlayerCharacter>(OutHit.GetActor());
			if (HitCharacter)
			{
				HitCharacter->OnWeaponHitEvent(OutHit);
			}
			else if (ABasicMonsterAI* HitMonster = Cast<ABasicMonsterAI>(OutHit.GetActor()))
			{
				HitMonster->OnWeaponHitEvent(OutHit);
			}
		}

		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}

		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}