// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\Monster\BasicMonsterAI.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	// 1. ��� Ŭ���̾�Ʈ�� �������� �߻� ȿ��(�ִϸ��̼�, ����, ȭ�� ��鸲, źȯ �Ҹ�)�� ���� �����մϴ�.
	//    Ŭ���̾�Ʈ������ �� �κ��� '����'���� ��� ����˴ϴ�.
	Super::Fire(HitTarget);

	// 2. ���� ����ü ���� ������ ���� ���������� ����ǵ��� �����ϴ�.
	if (!HasAuthority())
	{
		return;
	}

	// 3. ���� ������ ���������� ����˴ϴ�.
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

		// ������ '��¥' �����Ǵ� ����ü�� �����մϴ�.
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