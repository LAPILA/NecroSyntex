// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"
#include "NecroSyntex/Weapon/Weapon.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "NecroSyntex/Character/PlayerAnimInstance.h"
#include "NecroSyntex/Weapon/Projectile.h"
#include "NecroSyntex/Weapon/Shotgun.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, PrimaryWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, ThirdWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);

	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, Grenades);
}

void UCombatComponent::PickUpAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitalizeCarriedAmmo();
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if (Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.75f;
			switch (EquippedWeapon->FireType)
			{
			case EFireType::EFT_Projectile:
				FireProjectileWeapon();
				break;
			case EFireType::EFT_HitScan:
				FireHitScanWeapon();
				break;
			case EFireType::EFT_Shotgun:
				FireShotgun();
				break;
			}
		}
		StartFireTimer();
	}
}

void UCombatComponent::FireProjectileWeapon()
{
	if (EquippedWeapon && Character)
	{
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}

void UCombatComponent::FireHitScanWeapon()
{
	if (EquippedWeapon && Character)
	{
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}

void UCombatComponent::FireShotgun()
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);
	if (Shotgun && Character)
	{
		TArray<FVector_NetQuantize> HitTargets;
		Shotgun->ShotgunTraceEndWithScatter(HitTarget, HitTargets);
		if (!Character->HasAuthority()) ShotgunLocalFire(HitTargets);
		ServerShotgunFire(HitTargets, EquippedWeapon->FireDelay);
	}
}

bool UCombatComponent::CanFire()
{
	if (!EquippedWeapon) return false;

	// Special check for Shotgun reloading
	if (!EquippedWeapon->IsEmpty() && bCanFire &&
		CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		return true;
	}

	if (bLocallyReloading) return false;

	// Normal check: must be Unoccupied, not empty, bCanFire = true
	return (!EquippedWeapon->IsEmpty()
		&& bCanFire
		&& CombatState == ECombatState::ECS_Unoccupied);
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = (Controller == nullptr) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotgunEnd =
		CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun &&
		CarriedAmmo == 0;

	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::InitalizeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
}

void UCombatComponent::StartFireTimer()
{
	if (!EquippedWeapon || !Character) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (!EquippedWeapon) return;

	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon && !SecondaryWeapon)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	DropEquippedWeapon();

	PrimaryWeapon = WeaponToEquip;
	PrimaryWeapon->SetOwner(Character);

	EquippedWeapon = PrimaryWeapon;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	AttachActorToRightHand(EquippedWeapon);

	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(EquippedWeapon);
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);

	AttachActorToBackPack(SecondaryWeapon);
	PlayEquipWeaponSound(SecondaryWeapon);
	SecondaryWeapon->SetOwner(Character);
}

void UCombatComponent::EquipThirdWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	ThirdWeapon = WeaponToEquip;
	ThirdWeapon->SetOwner(Character);
	ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);

	AttachActorToBackPack2(ThirdWeapon);
	PlayEquipWeaponSound(ThirdWeapon);
}

bool UCombatComponent::ShouldSwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return false;

	return (
		EquippedWeapon != nullptr &&
		(PrimaryWeapon != nullptr || SecondaryWeapon != nullptr || ThirdWeapon != nullptr)
		);
}


void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (!EquippedWeapon) return 0;

	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (CombatState != ECombatState::ECS_Unoccupied || !EquippedWeapon) return;

	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed * Character->GrenadeThrowSpeedMultiplier;
	}

	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (!Character || !Character->GetMesh() || !ActorToAttach) return;

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if (!Character || !Character->GetMesh() || !ActorToAttach || !EquippedWeapon) return;

	bool bUsePistolSocket =
		(EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
			EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SubmachineGun);

	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBackPack(AActor* ActorToAttach)
{
	if (!Character || !Character->GetMesh() || !ActorToAttach) return;

	const USkeletalMeshSocket* BackPackSocket = Character->GetMesh()->GetSocketByName(FName("BackPacksocket"));
	if (BackPackSocket)
	{
		BackPackSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBackPack2(AActor* ActorToAttach)
{
	if (!Character || !Character->GetMesh() || !ActorToAttach) return;

	const USkeletalMeshSocket* BackPack2Socket = Character->GetMesh()->GetSocketByName(FName("BackPacksocket2"));
	if (BackPack2Socket)
	{
		BackPack2Socket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (!EquippedWeapon) return;

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = (Controller == nullptr) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayEquipWeaponSound(AWeapon* WeaponToEquip)
{
	if (Character && WeaponToEquip && WeaponToEquip->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponToEquip->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::ReloadEmptyWeapon()
{
	if (EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::CancelReload()
{
	if (CombatState == ECombatState::ECS_Reloading)
	{
		if (Character && Character->HasAuthority())
		{
			ServerCancelReload();
		}
		else
		{
			ServerCancelReload();
		}
	}
}

void UCombatComponent::ServerCancelReload_Implementation()
{
	MulticastCancelReload();
}

void UCombatComponent::MulticastCancelReload_Implementation()
{
	if (CombatState == ECombatState::ECS_Reloading)
	{
		CombatState = ECombatState::ECS_Unoccupied;
		bLocallyReloading = false;
		if (Character)
		{
			UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
			if (AnimInstance && Character->GetReloadMontage())
			{
				AnimInstance->Montage_Stop(0.1f, Character->GetReloadMontage());
			}
		}
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = (Controller == nullptr) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayEquipWeaponSound(EquippedWeapon);

		EquippedWeapon->EnableCustomDepth(false);
		EquippedWeapon->SetHUDAmmo();
	}
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(SecondaryWeapon);
		PlayEquipWeaponSound(SecondaryWeapon);
	}
}

void UCombatComponent::OnRep_ThirdWeapon()
{
	if (ThirdWeapon && Character)
	{
		ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
		AttachActorToBackPack2(ThirdWeapon);
		PlayEquipWeaponSound(ThirdWeapon);
	}
}

void UCombatComponent::CycleWeapons()
{
	if (!Character || !EquippedWeapon) return;

	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (!ShouldSwapWeapons()) return;

	CombatState = ECombatState::ECS_SwappingWeapons;
	Character->bFinishedSwapping = false;

	if (Character->IsLocallyControlled())
	{
		Character->PlaySwapMontage();
	}

	MulticastPlaySwapMontage();

	if (Character->HasAuthority())
	{
		CycleWeaponsLogic();
	}
}

void UCombatComponent::MulticastPlaySwapMontage_Implementation()
{
	if (!Character) return;

	if (!Character->IsLocallyControlled() || Character->HasAuthority())
	{
		Character->PlaySwapMontage();
	}
}


void UCombatComponent::ServerCycleWeapons_Implementation()
{
	CycleWeapons();
}

void UCombatComponent::CycleWeaponsLogic()
{
	if (EquippedWeapon == PrimaryWeapon && SecondaryWeapon)
	{
		PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(PrimaryWeapon);

		EquippedWeapon = SecondaryWeapon;
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(SecondaryWeapon);

		if (ThirdWeapon)
		{
			ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
			AttachActorToBackPack2(ThirdWeapon);
		}
	}
	else if (EquippedWeapon == SecondaryWeapon && ThirdWeapon)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(SecondaryWeapon);

		EquippedWeapon = ThirdWeapon;
		ThirdWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(ThirdWeapon);

		if (PrimaryWeapon)
		{
			PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
			AttachActorToBackPack2(PrimaryWeapon);
		}
	}
	else if (EquippedWeapon == ThirdWeapon && PrimaryWeapon)
	{
		ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
		AttachActorToBackPack2(ThirdWeapon);

		EquippedWeapon = PrimaryWeapon;
		PrimaryWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(PrimaryWeapon);

		if (SecondaryWeapon)
		{
			SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
			AttachActorToBackPack(SecondaryWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid weapon to cycle."));
		return;
	}

	bCanFire = true;
	bLocallyReloading = false;
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);

	PlayEquipWeaponSound(EquippedWeapon);
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();

	// 무기 스왑 상태 해제
	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 &&
		CombatState == ECombatState::ECS_Unoccupied &&
		EquippedWeapon &&
		!EquippedWeapon->IsFull() &&
		!bLocallyReloading)
	{
		ServerReload();
		HandleReload();
		bLocallyReloading = true;
	}
}

void UCombatComponent::FinishReloading()
{
	if (!Character) return;

	bLocallyReloading = false;

	if (Character->bIsCrouched)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->CrouchSpeed;
	}
	else if (bAiming)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->AimWalkSpeed;
	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;
	}

	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
}


void UCombatComponent::FinishSwap()
{
	if (Character && Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (Character) Character->bFinishedSwapping = true;
}

void UCombatComponent::FinishSwapAttachWeapons()
{
	if (!EquippedWeapon || !Character) return;

	// EquippedWeapon 기준으로 무기 순환 처리
	if (EquippedWeapon == PrimaryWeapon && SecondaryWeapon)
	{
		// Primary → BackPack
		PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(PrimaryWeapon);

		// Secondary → Equipped (RightHand)
		EquippedWeapon = SecondaryWeapon;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

		// Third → BackPack2
		if (ThirdWeapon)
		{
			ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
			AttachActorToBackPack2(ThirdWeapon);
		}
	}
	else if (EquippedWeapon == SecondaryWeapon && ThirdWeapon)
	{
		// Secondary → BackPack
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(SecondaryWeapon);

		// Third → Equipped (RightHand)
		EquippedWeapon = ThirdWeapon;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

		// Primary → BackPack2
		if (PrimaryWeapon)
		{
			PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
			AttachActorToBackPack2(PrimaryWeapon);
		}
	}
	else if (EquippedWeapon == ThirdWeapon && PrimaryWeapon)
	{
		// Third → BackPack2
		ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
		AttachActorToBackPack2(ThirdWeapon);

		// Primary → Equipped (RightHand)
		EquippedWeapon = PrimaryWeapon;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

		// Secondary → BackPack
		if (SecondaryWeapon)
		{
			SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
			AttachActorToBackPack(SecondaryWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FinishSwapAttachWeapons() failed: invalid weapons state."));
		return;
	}

	// 부가적인 후속 처리
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(EquippedWeapon);
}


void UCombatComponent::ServerReload_Implementation()
{
	if (!Character || !EquippedWeapon) return;

	CombatState = ECombatState::ECS_Reloading;
	if (!Character->IsLocallyControlled())
	{
		HandleReload();
	}
}


void UCombatComponent::HandleReload()
{
	if (Character && EquippedWeapon && !EquippedWeapon->IsFull())
	{
		Character->PlayReloadMontage();
		bLocallyReloading = true;

		Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed * Character->ReloadSpeedMultiplier;
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (!Character || !EquippedWeapon) return;

	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32& CarriedRef = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		CarriedRef = FMath::Max(0, CarriedRef - ReloadAmount);
		CarriedAmmo = CarriedRef;
	}

	Controller = (!Controller) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (!Character || !EquippedWeapon) return;

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32& CarriedRef = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		CarriedRef = FMath::Max(0, CarriedRef - 1);
		CarriedAmmo = CarriedRef;
	}

	Controller = (!Controller) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(1);
	bCanFire = true;

	if (EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::JumpToShotgunEnd()
{
	if (!Character) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (Character->GetReloadMontage() &&
		AnimInstance->Montage_IsPlaying(Character->GetReloadMontage()))
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"), Character->GetReloadMontage());
	}

	bLocallyReloading = false;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRightHand(EquippedWeapon);
}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);
	if (Character->bIsCrouched)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->CrouchSpeed;
	}
	else if (bAiming)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->AimWalkSpeed;
	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;
	}
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (Character && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				GrenadeClass,
				StartingLocation,
				ToTarget.Rotation(),
				SpawnParams
			);
		}
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled())
		{
			HandleReload();
		}
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
		}
		break;
	case ECombatState::ECS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlaySwapMontage();
		}
		break;
	}
}

bool UCombatComponent::ServerFire_Validate(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (!EquippedWeapon) return false;

	float ServerDelay = EquippedWeapon->FireDelay;
	bool bNearlyEqual = FMath::IsNearlyEqual(ServerDelay, FireDelay, 0.001f);
	return bNearlyEqual;
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (!EquippedWeapon || !Character) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float FireDelaySec = EquippedWeapon->FireDelay;
	const float Tolerance = 0.02f;

	if ((CurrentTime - LastServerFireTime + Tolerance) < FireDelaySec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CHEAT DETECTED?] %s fired too quickly (%.3f s < %.3f s)"),
			*Character->GetName(),
			CurrentTime - LastServerFireTime,
			FireDelaySec
		);
		return;
	}

	LastServerFireTime = CurrentTime;
	MulticastFire(TraceHitTarget);
}

bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (!EquippedWeapon) return false;
	float ServerDelay = EquippedWeapon->FireDelay;
	return FMath::IsNearlyEqual(ServerDelay, FireDelay, 0.001f);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalFire(TraceHitTarget);
}

void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (!EquippedWeapon || !Character) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float FireDelaySec = EquippedWeapon->FireDelay;
	const float Tolerance = 0.02f;

	if ((CurrentTime - LastServerShotgunFireTime + Tolerance) < FireDelaySec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CHEAT DETECTED?] %s fired shotgun too quickly (%.3f s < %.3f s)"),
			*Character->GetName(),
			CurrentTime - LastServerShotgunFireTime,
			FireDelaySec
		);
		return;
	}

	LastServerShotgunFireTime = CurrentTime;
	MulticastShotgunFire(TraceHitTargets);
}

void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	ShotgunLocalFire(TraceHitTargets);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (!EquippedWeapon) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);
	if (!Shotgun || !Character) return;

	if (CombatState == ECombatState::ECS_Reloading || CombatState == ECombatState::ECS_Unoccupied)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		Shotgun->FireShotgun(TraceHitTargets);
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	if (!Character) return;

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Character);
		if (Character->GetEquippedWeapon())
		{
			QueryParams.AddIgnoredActor(Character->GetEquippedWeapon());
		}

		const FVector Start = CrosshairWorldPosition;
		const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility,
			QueryParams
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() &&
			TraceHitResult.GetActor()->Implements<UInteractWithCrossHairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (!Character || !Character->Controller) return;

	Controller = (!Controller) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = (!HUD) ? Cast<ANecroSyntexHud>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}

			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (!EquippedWeapon) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			EquippedWeapon->GetZoomedFOV(),
			DeltaTime,
			EquippedWeapon->GetZoomedInterpedSpeed()
		);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (!Character || !EquippedWeapon) return;

	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);

	if (bAiming)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->AimWalkSpeed;
	}
	else
	{
		if (Character->bIsCrouched)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->CrouchSpeed;
		}
		else
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;
		}
	}
}


void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed =
			bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}
