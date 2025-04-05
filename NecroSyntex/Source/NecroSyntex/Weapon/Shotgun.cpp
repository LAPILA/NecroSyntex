#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "NecroSyntex/NecroSyntaxComponents/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to number of times hit
		TMap<APlayerCharacter*, uint32> HitMap;
		TMap<APlayerCharacter*, uint32> HeadShotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FireHit.GetActor());
			if (PlayerCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(PlayerCharacter)) HeadShotHitMap[PlayerCharacter]++;
					else HeadShotHitMap.Emplace(PlayerCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(PlayerCharacter)) HitMap[PlayerCharacter]++;
					else HitMap.Emplace(PlayerCharacter, 1);
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
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f, .5f)
					);
				}
			}

			ABasicMonsterAI* MonsterCharacter = Cast<ABasicMonsterAI>(FireHit.GetActor());
			if (MonsterCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head"); // 없어도 문제 없음

				const float DamageToCause = bHeadShot ? HeadShotDamage : Damage;

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
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f, .5f)
					);
				}

				if (InstigatorController && OwnerPawn->IsLocallyControlled())
				{
					bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
					if (HasAuthority() && bCauseAuthDamage)
					{
						UGameplayStatics::ApplyDamage(
							MonsterCharacter,
							DamageToCause,
							InstigatorController,
							this,
							UDamageType::StaticClass()
						);
					}
					else if (InstigatorController) {
						Server_ApplyMonsterDamage(MonsterCharacter, DamageToCause, InstigatorController);
					}
				}
			}

		}
		TArray<APlayerCharacter*> HitCharacters;

		// Maps Character hit to total damage
		TMap<APlayerCharacter*, float> DamageMap;

		// Calculate body shot damage by multiplying times hit x Damage - store in DamageMap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// Calculate head shot damage by multiplying times hit x HeadShotDamage - store in DamageMap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		// Loop through DamageMap to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key, // Character that was hit
						DamagePair.Value, // Damage calculated in the two for loops above
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}


		if (!HasAuthority() && bUseServerSideRewind)
		{
			PlayerOwnerCharacter = PlayerOwnerCharacter == nullptr ? Cast<APlayerCharacter>(OwnerPawn) : PlayerOwnerCharacter;
			NecroSyntexPlayerOwnerController = NecroSyntexPlayerOwnerController == nullptr ? Cast<ANecroSyntexPlayerController>(InstigatorController) : NecroSyntexPlayerOwnerController;
			if (NecroSyntexPlayerOwnerController && PlayerOwnerCharacter && PlayerOwnerCharacter->GetLagCompensation() && PlayerOwnerCharacter->IsLocallyControlled())
			{
				PlayerOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					NecroSyntexPlayerOwnerController->GetServerTime() - NecroSyntexPlayerOwnerController->SingleTripTime
				);
			}
		}
	}

}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket == nullptr) return;

    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation();

    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

    // 펠릿을 여러 개 산포
    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        const FVector EndLoc = SphereCenter + RandVec;
        FVector ToEndLoc = EndLoc - TraceStart;
        // Distance를 그대로 늘려서 실제 트레이스 끝 점 구함
        ToEndLoc = TraceStart + ToEndLoc * (TRACE_LENGTH / ToEndLoc.Size());

        HitTargets.Add(ToEndLoc);
    }
}
