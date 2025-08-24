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
#include "NecroSyntex/Voice/VoiceComponent.h"
#include "NecroSyntex/NecroSyntaxComponents/DR_FlashDrone.h"

#define TRY_PLAY_VOICE(Cue)  Character->GetVoiceComp()->PlayVoice(Cue)
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

	SwapCooldownTime = 0.3f;
	bCanSwapWeapon = true;
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

		if (ADR_FlashDrone* Drone = Character->GetFlashDrone())
		{
			Drone->SetAimTarget(HitTarget);
		}
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
	if (CombatState == ECombatState::ECS_ThrowingGrenade) return;

	// 무기 전환 중에는 발사하지 않음
	if (!bCanFire || CombatState == ECombatState::ECS_SwappingWeapons) return;

	if (CanFire())
	{
		TRY_PLAY_VOICE(EVoiceCue::Fire);
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

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun && CombatState == ECombatState::ECS_Reloading)
	{
		return !EquippedWeapon->IsEmpty() && bCanFire;
	}

	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied)
	{
		return true;
	}

	if (EquippedWeapon->IsEmpty() && CarriedAmmo == 0)
	{
		TRY_PLAY_VOICE(EVoiceCue::NoAmmo);
	}

	return false;
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

	//duream code start.
	weaponSlotNumber = GetWeaponSlot();

	if (weaponSlotNumber == 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR~~~~~~~~~`"));
		return;
	}

	if (EquippedWeapon && !SecondaryWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("second"));
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else if (SecondaryWeapon && !ThirdWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("third"));
		EquipThirdWeapon(WeaponToEquip);
	}
	else if (PrimaryWeapon && SecondaryWeapon && ThirdWeapon) {
		switch (weaponSlotNumber)
		{
		case 1:
			EquipPrimaryWeapon(WeaponToEquip);
			break;
		case 2:
			SecondaryWeapon->Dropped();
			EquipSecondaryWeapon(WeaponToEquip);
			break;
		case 3:
			ThirdWeapon->Dropped();
			EquipThirdWeapon(WeaponToEquip);
		default:
			break;
		}
	}
	else {
		//debug message.
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("not ~~~~~~~~~~~~ equippedweapon && !secondaryweapon"));
		EquipPrimaryWeapon(WeaponToEquip);
	}
	//duream code end.
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	NotifyWeaponChanged(EquippedWeapon);
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;
	
	//debug message.
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("equip primaryweapon"));
	
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
	//duream code start.
	if (PrimaryWeapon && SecondaryWeapon && !ThirdWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("equipsecondary"));
		AttachActorToBackPack(SecondaryWeapon);
		PlayEquipWeaponSound(SecondaryWeapon);
		SecondaryWeapon->SetOwner(Character);
	
		return;
	}
	else if (PrimaryWeapon && SecondaryWeapon && ThirdWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("equipsecondary22222222222"));
		EquippedWeapon = SecondaryWeapon;
		AttachActorToRightHand(EquippedWeapon);
		EquippedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
		PlayEquipWeaponSound(SecondaryWeapon);
		SecondaryWeapon->SetOwner(Character);

		return;
	}
	//duream code end.
}

void UCombatComponent::EquipThirdWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	ThirdWeapon = WeaponToEquip;
	ThirdWeapon->SetWeaponState(EWeaponState::EWS_EquippedThird);
	//duream code start.
	if (PrimaryWeapon && SecondaryWeapon && ThirdWeapon && !isThirdWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("equipthird"));
		isThirdWeapon = true;
		AttachActorToBackPack2(ThirdWeapon);
		PlayEquipWeaponSound(ThirdWeapon);
		ThirdWeapon->SetOwner(Character);

		return;
	}
	else if (PrimaryWeapon && SecondaryWeapon && ThirdWeapon && isThirdWeapon) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("equipthird33333333333333"));
		EquippedWeapon = ThirdWeapon;
		AttachActorToRightHand(EquippedWeapon);
		EquippedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
		PlayEquipWeaponSound(ThirdWeapon);
		ThirdWeapon->SetOwner(Character);

		return;
	}

	//duream code end.
}

void UCombatComponent::SwapWeaponByNumber(int32 WeaponNumber)
{
	if (!Character || !bCanSwapWeapon) return;

	AWeapon* NewWeapon = nullptr;

	switch (WeaponNumber)
	{
	case 1:
		NewWeapon = PrimaryWeapon;
		break;
	case 2:
		NewWeapon = SecondaryWeapon;
		break;
	case 3:
		NewWeapon = ThirdWeapon;
		break;
	default:
		return;
	}

	// 무기 전환 중이거나 동일 무기 교체 시 무시
	if (NewWeapon == EquippedWeapon || CombatState != ECombatState::ECS_Unoccupied) return;

	// 무기 전환 전 상태 강제 초기화
	ResetFireState();

	if (Character->HasAuthority())
	{
		AttachActorToBackPack(EquippedWeapon);
		EquippedWeapon = NewWeapon;
		AttachActorToRightHand(EquippedWeapon);
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		// 무기 교체 후 발사 상태 초기화
		ResetFireState();

		// HUD 갱신
		NotifyWeaponChanged(EquippedWeapon);
		PlayEquipWeaponSound(EquippedWeapon);
		UpdateCarriedAmmo();

		// 무기 교체 쿨다운 시작
		StartWeaponSwapCooldown();
		bCanSwapWeapon = true;
	}
	else
	{
		ServerSwapWeaponByNumber(WeaponNumber);
	}
}

void UCombatComponent::ServerSwapWeaponByNumber_Implementation(int32 WeaponNumber)
{
	SwapWeaponByNumber(WeaponNumber);
}

void UCombatComponent::MulticastSwapWeaponByNumber_Implementation(int32 WeaponNumber)
{
	if (!Character->IsLocallyControlled())
	{
		SwapWeaponByNumber(WeaponNumber);
	}
}

void UCombatComponent::ResetFireState()
{
	bCanFire = true;
	bFireButtonPressed = false;

	if (Character)
	{
		Character->bFinishedSwapping = true;

		if (Character->bIsCrouched)
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->CrouchSpeed;
		else if (bAiming)
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->AimWalkSpeed;
		else
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;

		if (Character->GetFollowCamera())
			Character->GetFollowCamera()->SetFieldOfView(DefaultFOV);
	}
}

void UCombatComponent::StartWeaponSwapCooldown()
{
	bCanSwapWeapon = false;
	GetWorld()->GetTimerManager().SetTimer(
		SwapCooldownTimer,
		this,
		&UCombatComponent::ResetWeaponSwapCooldown,
		SwapCooldownTime,
		false
	);
}

void UCombatComponent::ResetWeaponSwapCooldown()
{
	bCanSwapWeapon = true;
}


void UCombatComponent::FinishWeaponSwap()
{
	bCanFire = true;
	bFireButtonPressed = false;

	// 무기 교체 후 발사 상태 초기화
	ResetFireState();
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
	bCanFire = false;

	TRY_PLAY_VOICE(EVoiceCue::ThrowGrenade);

	if (Character)
	{
		AttachActorToLeftHand(EquippedWeapon);
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed * Character->GrenadeThrowSpeedMultiplier;
	}

	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	bCanFire = true;
	AttachActorToRightHand(EquippedWeapon);
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

	if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(ActorToAttach->GetRootComponent()))
	{
		RootComp->SetSimulatePhysics(false);
		RootComp->SetEnableGravity(false);
		RootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

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
		(EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol);

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

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		AttachActorToLeftHand(EquippedWeapon);
	}
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = (Controller == nullptr) ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
}

void UCombatComponent::RemoveGrenade(int32 Amount)
{
	if (Grenades <= 0 || Amount <= 0) return;

	Grenades = FMath::Max(Grenades - Amount, 0);
	UpdateHUDGrenades();
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

		if (Character->IsLocallyControlled())
		{
			NotifyWeaponChanged(EquippedWeapon);
		}

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

void UCombatComponent::CycleWeaponsLogic()
{
	if (!EquippedWeapon) return;

	int32 NextWeaponNumber = 1;

	// 정확한 무기 순환 순서 설정
	if (EquippedWeapon == PrimaryWeapon)
		NextWeaponNumber = (SecondaryWeapon != nullptr) ? 2 : (ThirdWeapon != nullptr) ? 3 : 1;
	else if (EquippedWeapon == SecondaryWeapon)
		NextWeaponNumber = (ThirdWeapon != nullptr) ? 3 : (PrimaryWeapon != nullptr) ? 1 : 2;
	else if (EquippedWeapon == ThirdWeapon)
		NextWeaponNumber = (PrimaryWeapon != nullptr) ? 1 : (SecondaryWeapon != nullptr) ? 2 : 3;

	// 무기 교체 호출
	SwapWeaponByNumber(NextWeaponNumber);

	// 교체 후 발사 가능 상태 설정
	bCanFire = true;
}

void UCombatComponent::NotifyWeaponChanged(AWeapon* NewWeapon)
{
	if (!Character || !NewWeapon) return;

	ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(Character->Controller);
	if (PC)
	{
		PC->SetHUDWeaponImage(NewWeapon->GetWeaponImage());
		PC->SetHUDWeaponAmmo(NewWeapon->GetAmmo());
		PC->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::MulticastNotifyWeaponChanged_Implementation(AWeapon* NewWeapon)
{
	NotifyWeaponChanged(NewWeapon);
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && EquippedWeapon && !EquippedWeapon->IsFull())
	{
		ServerReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if (!Character) return;

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
	if (!Character || !Character->HasAuthority()) return;

	if (!Character || !GetWorld())
	{
		return;
	}

	CombatState = ECombatState::ECS_Unoccupied;

	if (Character)
	{
		Character->bFinishedSwapping = true;

		// 무기 변경 후 HUD 갱신
		NotifyWeaponChanged(EquippedWeapon);

		if (Character->IsLocallyControlled())
		{
			MulticastNotifyWeaponChanged(EquippedWeapon);
		}
	}

	// 무기 교체 후 발사 상태 강제 초기화
	ResetFireState();

	// FireTimer 강제 초기화
	if (GetWorld()->GetTimerManager().IsTimerActive(FireTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	}

	GetWorld()->GetTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		0.0f,   // 즉시 발사 가능
		false
	);
}

void UCombatComponent::FinishSwapAttachWeapons()
{
	if (!EquippedWeapon || !Character) return;
	ResetFireState();
	if (EquippedWeapon == PrimaryWeapon && SecondaryWeapon)
	{
		PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackPack(PrimaryWeapon);

		EquippedWeapon = SecondaryWeapon;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

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
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

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
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);

		if (SecondaryWeapon)
		{
			SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
			AttachActorToBackPack(SecondaryWeapon);
		}
	}
	else
	{
		return;
	}

	// 부가적인 후속 처리
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(EquippedWeapon);

	// 무기 교체 후 HUD 업데이트
	NotifyWeaponChanged(EquippedWeapon);
}

void UCombatComponent::ServerReload_Implementation()
{
	if (!Character || !EquippedWeapon) return;

	if (CombatState == ECombatState::ECS_Unoccupied && CarriedAmmo > 0 && !EquippedWeapon->IsFull())
	{
		CombatState = ECombatState::ECS_Reloading;

		HandleReload();
	}
}

void UCombatComponent::HandleReload()
{
	if (Character && EquippedWeapon && !EquippedWeapon->IsFull())
	{
		Character->PlayReloadMontage();

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

	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::LaunchGrenade()
{
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
		if (Character)
		{
			HandleReload();
		}
		break;

	case ECombatState::ECS_Unoccupied:
		if (Character && Character->GetCharacterMovement())
		{
			if (bAiming)
			{
				Character->GetCharacterMovement()->MaxWalkSpeed = Character->AimWalkSpeed;
			}
			else
			{
				Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;
			}
		}
		break;

	case ECombatState::ECS_ThrowingGrenade:
		if (Character && EquippedWeapon)
		{
			AttachActorToLeftHand(EquippedWeapon);
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
	if (!IsValid(EquippedWeapon) || !IsValid(Character))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	const float FireDelaySec = EquippedWeapon->FireDelay;
	const float Tolerance = 0.02f;
	const float ClientTimeDifference = FMath::Abs(CurrentTime - LastServerFireTime);

	if ((ClientTimeDifference + Tolerance) < FireDelaySec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CHEAT DETECTED?] %s fired too quickly (%.3f s < %.3f s)"),
			*Character->GetName(),
			ClientTimeDifference,
			FireDelaySec
		);
		return;
	}

	LastServerFireTime = CurrentTime;

	EquippedWeapon->SpendRound();
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
		else if (Character->bWantsToSprint)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->RunningSpeed;
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

void UCombatComponent::GetOwnedWeaponTypes(TArray<EWeaponType>& OutWeaponTypes) const
{
	if (PrimaryWeapon) OutWeaponTypes.Add(PrimaryWeapon->GetWeaponType());
	if (SecondaryWeapon) OutWeaponTypes.Add(SecondaryWeapon->GetWeaponType());
	if (ThirdWeapon) OutWeaponTypes.Add(ThirdWeapon->GetWeaponType());
}

int32 UCombatComponent::GetMaxAmmoForWeaponType(EWeaponType WeaponType) const
{
	// 무기 타입에 따라 최대 탄약 수량 설정
	switch (WeaponType)
	{
	case EWeaponType::EWT_AssaultRifle: return 30;
	case EWeaponType::EWT_Pistol: return 15;
	case EWeaponType::EWT_Shotgun: return 8;
	case EWeaponType::EWT_SubmachineGun: return 25;
	case EWeaponType::EWT_SniperRifle: return 5;
	case EWeaponType::EWT_RocketLauncher: return 1;
	case EWeaponType::EWT_GrenadeLauncher: return 3;
	default: return 30;
	}
}

int32 UCombatComponent::GetWeaponSlot() const
{
	if (EquippedWeapon == PrimaryWeapon) {
		return 1;
	}
	else if (EquippedWeapon == SecondaryWeapon) {
		return 2;
	}
	else if (EquippedWeapon == ThirdWeapon) {
		return 3;
	}
	return 0;
}
