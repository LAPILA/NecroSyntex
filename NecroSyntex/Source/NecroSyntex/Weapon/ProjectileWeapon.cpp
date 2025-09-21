// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\Monster\BasicMonsterAI.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	// 1. 모든 클라이언트와 서버에서 발사 효과(애니메이션, 사운드, 화면 흔들림, 탄환 소모)를 먼저 실행합니다.
	//    클라이언트에서는 이 부분이 '예측'으로 즉시 실행됩니다.
	Super::Fire(HitTarget);

	// 2. 실제 투사체 스폰 로직은 오직 서버에서만 실행되도록 막습니다.
	if (!HasAuthority())
	{
		return;
	}

	// 3. 이하 로직은 서버에서만 실행됩니다.
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (!InstigatorPawn) return;

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 서버가 '진짜' 복제되는 투사체를 스폰합니다.
		AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
			ProjectileClass,
			SocketTransform.GetLocation(),
			TargetRotation,
			SpawnParams
		);

		if (SpawnedProjectile)
		{
			SpawnedProjectile->Damage = Damage;
			SpawnedProjectile->HeadShotDamage = HeadShotDamage;
			SpawnedProjectile->SubDamage = SubDamage;
		}
	}
}