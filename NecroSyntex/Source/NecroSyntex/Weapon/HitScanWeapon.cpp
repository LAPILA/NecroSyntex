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

#include "DrawDebugHelpers.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FireHit.GetActor());

		ABasicMonsterAI* MonsterCharacter = Cast<ABasicMonsterAI>(FireHit.GetActor());

		if (MonsterCharacter && InstigatorController && OwnerPawn->IsLocallyControlled())
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
				UGameplayStatics::ApplyDamage(
					MonsterCharacter,
					DamageToCause,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			else if (!HasAuthority())
			{
				const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
				Server_ApplyMonsterDamage(MonsterCharacter, DamageToCause, InstigatorController);
			}
		}

		if (PlayerCharacter && InstigatorController && OwnerPawn->IsLocallyControlled())
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority()&& bCauseAuthDamage)
			{
				const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
				UGameplayStatics::ApplyDamage(
					PlayerCharacter,
					DamageToCause,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			if (!HasAuthority() && bUseServerSideRewind)
			{
				PlayerOwnerCharacter = PlayerOwnerCharacter == nullptr ? Cast<APlayerCharacter>(OwnerPawn) : PlayerOwnerCharacter;
				NecroSyntexPlayerOwnerController = NecroSyntexPlayerOwnerController == nullptr ? Cast<ANecroSyntexPlayerController>(InstigatorController) : NecroSyntexPlayerOwnerController;
				if (NecroSyntexPlayerOwnerController && PlayerOwnerCharacter && PlayerOwnerCharacter->GetLagCompensation() && PlayerOwnerCharacter->IsLocallyControlled())
				{
					PlayerOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						PlayerCharacter,
						Start,
						HitTarget,
						NecroSyntexPlayerOwnerController->GetServerTime() - NecroSyntexPlayerOwnerController->SingleTripTime
					);
				}
			}
		}

		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint
			);
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		// 1) ���� Ʈ���̽� ���� ���
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

		// 2) �ڱ� �ڽ� �Ǵ� Owner�� �����ϱ� ���� QueryParams ����
		FCollisionQueryParams QueryParams;
		QueryParams.bReturnPhysicalMaterial = false; // �ʿ信 ���� ���
		// ������ �ڽ�(Weapon ����)�� Owner(�÷��̾� Pawn)�� Ignore
		QueryParams.AddIgnoredActor(this);
		if (AActor* MyOwner = GetOwner())
		{
			QueryParams.AddIgnoredActor(MyOwner);
		}

		// 3) ���� Ʈ���̽� ����
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Pawn,
			QueryParams
		);

		// 4) ���� ��ġ/����Ʈ ó��
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}

		// �� ����Ʈ ó��
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
