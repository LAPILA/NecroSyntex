#pragma region Includes
// Engine Includes
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// NecroSyntex Includes
#include "NecroSyntex/NecroSyntex.h"
#include "NecroSyntex/Weapon/Weapon.h"
#include "NecroSyntex/Weapon/WeaponTypes.h"
#include "NecroSyntex/NecroSyntaxComponents/CombatComponent.h"
#include "NecroSyntex/NecroSyntaxComponents/SubComponent.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "NecroSyntex/PlayerState/NecroSyntexPlayerState.h"
#include "NecroSyntex/DopingSystem/DopingComponent.h"
#include "NecroSyntex/DopingSystem/DeBuffCameraShake.h"
#include "NecroSyntex/NecroSyntaxComponents/LagCompensationComponent.h"
#include "NecroSyntex/PickUps/HealingStation.h"
#include "NecroSyntex\PickUps\SupplyCrate.h"

// Animation
#include "PlayerAnimInstance.h"
#pragma endregion

#define TRY_PLAY_VOICE(Cue)  if (VoiceComp) VoiceComp->PlayVoice((Cue))

#pragma region Constructor

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//================= Camera Setup =================
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 120.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//================= Character Movement ================
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	GetCharacterMovement()->AirControl = 0.3f;
	GetCharacterMovement()->MaxAcceleration = 4096.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;

	//================= Collision Settings ================
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	//================= Components ========================
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	SubComp = CreateDefaultSubobject<USubComponent>(TEXT("SubComponent"));
	SubComp->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	DissolveEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DissolveEffectComponent"));
	DissolveEffectComponent->SetupAttachment(GetMesh());

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//================= Hit Boxes for SSR ================
	TMap<FName, FName> BoneNames = {
		{"head", "head"}, {"pelvis", "pelvis"}, {"spine_02", "spine_02"},
		{"spine_03", "spine_03"}, {"upperarm_l", "upperarm_l"}, {"upperarm_r", "upperarm_r"},
		{"lowerarm_l", "lowerarm_l"}, {"lowerarm_r", "lowerarm_r"}, {"hand_l", "hand_l"},
		{"hand_r", "hand_r"}, {"back", "neck_01"}, {"thigh_l", "thigh_l"},
		{"thigh_r", "thigh_r"}, {"calf_l", "calf_l"}, {"calf_r", "calf_r"},
		{"foot_l", "foot_l"}, {"foot_r", "foot_r"}
	};

	for (auto& Pair : BoneNames)
	{
		UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(*Pair.Key.ToString());
		Box->SetupAttachment(GetMesh(), Pair.Value);
		Box->SetCollisionObjectType(ECC_HitBox);
		Box->SetCollisionResponseToAllChannels(ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitCollisionBoxes.Add(Pair.Key, Box);
	}

	HealingStationActor = nullptr;

	//================= Movement Speed ================
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	//================= Voice Pack ================
	VoiceComp = CreateDefaultSubobject<UVoiceComponent>(TEXT("VoiceComponent"));
	VoiceComp->SetIsReplicated(true);
}

#pragma endregion

#pragma region BeginPlay
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (VoiceComp && DefaultVoiceSet)
	{
		VoiceComp->VoiceSet = DefaultVoiceSet;
	}

	TRY_PLAY_VOICE(EVoiceCue::GameStart);

	// Input mapping
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSys->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Initialize weapons and HUD
	SpawnDefaultWeapon();
	UpdateHUDShield();
	UpdateHUDHealth();

	if (Combat && Combat->EquippedWeapon)
	{
		InitialCarriedAmmo = Combat->CarriedAmmo;
		InitialWeaponAmmo = Combat->EquippedWeapon->GetAmmo();
		bInitializeAmmo = true;
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}

	if (!UDC)
	{
		UDC = NewObject<UDopingComponent>(this);
		UDC->RegisterComponent();
		AddInstanceComponent(UDC);
	}

	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}
#pragma endregion

#pragma region Tick
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Clamp max speed
	FVector Velocity = GetCharacterMovement()->Velocity;
	if (Velocity.Size() > MaxCharacterSpeed)
	{
		GetCharacterMovement()->Velocity = Velocity.GetClampedToMaxSize(MaxCharacterSpeed);
	}

	// Rotate or calculate aim offset
	RotateInPlace(DeltaTime);

	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	if (!bIsCrouched)
	{
		HandleHeadBob(DeltaTime);
	}

	PollInit();
}
#pragma endregion

#pragma region PostInitializeComponents
void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->PrimaryComponentTick.bCanEverTick = true;
		Combat->Character = this;
	}

	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ANecroSyntexPlayerController>(Controller);
		}
	}
}
#pragma endregion

#pragma region Movement & Input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerCharacter::EquipButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintStop);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(FlashAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FlashButtonPressed);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(ThrowGrenade, ETriggerEvent::Triggered, this, &APlayerCharacter::GrenadeButtonPressed);
		EnhancedInputComponent->BindAction(SwapWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapWeaponWheel);
		EnhancedInputComponent->BindAction(UDCskill1, ETriggerEvent::Triggered, this, &APlayerCharacter::FirstDoping);
		EnhancedInputComponent->BindAction(UDCskill2, ETriggerEvent::Triggered, this, &APlayerCharacter::SecondDoping);
		EnhancedInputComponent->BindAction(UDCModeChange, ETriggerEvent::Triggered, this, &APlayerCharacter::DopingModeChange);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (ReservedMoving)
	{
		MovementVector.X *= -1;
		MovementVector.Y *= -1;
	}

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		float TargetSpeed = WalkSpeed;

		// 상태 기반
		if (bIsCrouched)
		{
			TargetSpeed = CrouchSpeed;
		}
		else if (Combat && Combat->bAiming)
		{
			TargetSpeed = AimWalkSpeed;
		}
		else if (IsLocallyReloading())
		{
			TargetSpeed = WalkSpeed * 0.6f;
		}
		else if (bIsSprinting || bWantsToSprint)
		{
			if (MovementVector.Y >= 0.5f)
			{
				bIsSprinting = true;
				TargetSpeed = RunningSpeed;
			}
			else
			{
				bIsSprinting = false;
				TargetSpeed = WalkSpeed;
			}
		}

		// 방향 감속
		if (!bIsSprinting)
		{
			if (MovementVector.Y < -0.1f)
			{
				TargetSpeed *= 0.6f;
			}
			else if (FMath::Abs(MovementVector.X) > 0.1f && FMath::Abs(MovementVector.Y) < 0.2f)
			{
				TargetSpeed *= 0.75f;
			}
		}
		else
		{
			if (MovementVector.Y < 0.5f)
			{
				TargetSpeed *= 0.9f;
			}
		}

		if (HasAuthority())
		{
			GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
		}
		else
		{
			static float SmoothedSpeed = WalkSpeed;
			const float InterpSpeed = 10.f;
			SmoothedSpeed = FMath::FInterpTo(SmoothedSpeed, TargetSpeed, GetWorld()->GetDeltaSeconds(), InterpSpeed);
			GetCharacterMovement()->MaxWalkSpeed = SmoothedSpeed;
		}

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::SprintStart()
{
	if (bDisableGameplay) return;

	bWantsToSprint = true;

	if (!bIsSprinting && GetVelocity().Size() > 0.f)
	{
		bIsSprinting = true;

		if (HasAuthority())
		{
			GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		}
		else
		{
			ServerSprintStart();
		}
	}
}

void APlayerCharacter::SprintStop()
{
	if (bDisableGameplay) return;

	bWantsToSprint = false;

	if (bIsSprinting)
	{
		bIsSprinting = false;

		if (HasAuthority())
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
		else
		{
			ServerSprintStop();
		}
	}
}

void APlayerCharacter::ServerSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void APlayerCharacter::ServerSprintStop_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

bool APlayerCharacter::ServerSprintStart_Validate()
{
	return true;
}

bool APlayerCharacter::ServerSprintStop_Validate()
{
	return true;
}

void APlayerCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;

	if (bIsCrouched)
	{
		UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		Crouch();
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	}
}
#pragma endregion

#pragma region Combat Actions
void APlayerCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (bElimed || bDisableGameplay)
	{
		return;
	}

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (bElimed || bDisableGameplay)
	{
		return;
	}

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(false);
	}
}

void APlayerCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

void APlayerCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

bool APlayerCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

void APlayerCharacter::ReloadTimerFinished()
{
	if (Combat)
	{
		Combat->FinishReloading();
	}
}

void APlayerCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (OverlappingSupplyCrate)
	{
		OverlappingSupplyCrate->Interact(this);
	}
	if (Combat)
	{
		ServerEquipButtonPressed();
	}
	if (HealingStationActor)
	{
		ServerRequestHealing();
	}
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
	}
}

void APlayerCharacter::SwapWeaponWheel()
{
	if (Combat)
	{
		Combat->CycleWeapons();
	}

	if (!HasAuthority())
	{
		ServerSwapWeaponWheel();
	}
}

bool APlayerCharacter::ServerSwapWeaponWheel_Validate()
{
	return true;
}

void APlayerCharacter::ServerSwapWeaponWheel_Implementation()
{
	if (Combat)
	{
		Combat->CycleWeapons();
	}
}

void APlayerCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}
void APlayerCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void APlayerCharacter::FlashButtonPressed()
{
}
#pragma endregion

#pragma region Animation
void APlayerCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage && HitReactMontage && !bElimed)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMongatge)
	{

		AnimInstance->Montage_Play(ElimMongatge);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NecroSyntexGamemode cannot find"));
	}
}

void APlayerCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void APlayerCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void APlayerCharacter::PlayDopingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DopingMontage)
	{
		AnimInstance->Montage_Play(DopingMontage);
	}
}

void APlayerCharacter::PlayerHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage
		&& !AnimInstance->IsAnyMontagePlaying() // 현재 몽타주 재생 중이면 HitReact는 재생하지 않음
		)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(FName("FromFront"));
	}
}

void APlayerCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);

		AnimInstance->Montage_Play(ReloadMontage);

		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
		case EWeaponType::EWT_RocketLauncher:
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("Shotgun");
			break;
		default:
			SectionName = FName("Rifle");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
	}
}

void APlayerCharacter::ReloadMontageEndedHandler(UAnimMontage* Montage, bool bInterrupted)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr || Combat == nullptr) return;

	if (bInterrupted)
	{
		float CurrentPosition = AnimInstance->Montage_GetPosition(ReloadMontage);
		FName SectionName = AnimInstance->Montage_GetCurrentSection();
		int32 SectionIndex = ReloadMontage->GetSectionIndex(SectionName);

		float OutStartTime, OutEndTime;
		ReloadMontage->GetSectionStartAndEndTime(SectionIndex, OutStartTime, OutEndTime);
		float TimeToWait = FMath::Clamp(OutEndTime - CurrentPosition, 0.f, OutEndTime - OutStartTime);

		if (TimeToWait > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				ReloadTimer,
				this,
				&APlayerCharacter::ReloadTimerFinished,
				TimeToWait
			);
		}
		else if (Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
		{
			Combat->JumpToShotgunEnd();
		}

		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);
	}
	else
	{
		Combat->FinishReloading();
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);
	}
}


#pragma endregion

#pragma region Damage & Elimination
void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimed)
	{
		return;
	}

	TRY_PLAY_VOICE(EVoiceCue::TakeHit);
	PlayerHitReactMontage();

	if (Combat && Combat->CombatState == ECombatState::ECS_Reloading)
	{
		Combat->CancelReload();
	}

	if (Shield > 0)
	{
		float NewShieldValue = FMath::Clamp(Shield - (Damage - Defense), 0.f, MaxShield);

		if (NewShieldValue == 0 && Shield != 0)
		{
			Shield = 0;
			UpdateHUDShield();
		}
		else if (NewShieldValue > 0)
		{
			Shield = NewShieldValue;
			UpdateHUDShield();
		}
	}
	else
	{
		Health = FMath::Clamp(Health - (Damage - Defense), 0.f, MaxHealth);
		UpdateHUDHealth();

		if (Health <= Health / 20) TRY_PLAY_VOICE(EVoiceCue::LowHP);

		if (Health == 0.0f)
		{
			PlayerDeathStopDopingEffect();
			UDC->One_Able = false;
			UDC->Two_Able = false;

			ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();
			if (NecroSyntexGameMode)
			{
				NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
				ANecroSyntexPlayerController* AttackerController = Cast<ANecroSyntexPlayerController>(InstigatorController);
				NecroSyntexGameMode->PlayerEliminated(this, NecroSyntexPlayerController, AttackerController);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NecroSyntexGamemode cannot find"));
			}
		}
	}
	if (SubComp)
	{
		SubComp->OnTakeDamage();
	}
}

void APlayerCharacter::Elim()
{
	DropOrDestroyWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&APlayerCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void APlayerCharacter::MulticastElim_Implementation()
{
	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimed = true;

	TRY_PLAY_VOICE(EVoiceCue::Death);
	PlayElimMontage();

	if (DissolveEffectComponent)
	{
		ActivateDissolveEffect();
	}

	// Disable character movement
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bHideSniperScope =
		IsLocallyControlled() &&
		Combat
		&& Combat->bAiming
		&& Combat->EquippedWeapon
		&& Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}
void APlayerCharacter::ElimTimerFinished()
{
	ANecroSyntexGameMode* NecroSyntexGameMode = GetWorld()->GetAuthGameMode<ANecroSyntexGameMode>();
	if (NecroSyntexGameMode)
	{
		NecroSyntexGameMode->RequestRespawn(this, Controller);
	}
}
#pragma endregion

#pragma region Weapon Handling
void APlayerCharacter::SpawnDefaultWeapon()
{
	if (!HasAuthority() || bElimed) return;
	ANecroSyntexGameMode* GameMode = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (!GameMode || !World || bElimed) return;

	// 1) Primary
	if (DefaultWeaponClass && Combat)
	{
		AWeapon* PrimaryWep = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		if (PrimaryWep)
		{
			PrimaryWep->bDestroyWeapon = true;
			Combat->EquipWeapon(PrimaryWep);
		}
	}
	// 2) Secondary
	if (SubWeaponClass && Combat)
	{
		AWeapon* SecondaryWep = World->SpawnActor<AWeapon>(SubWeaponClass);
		if (SecondaryWep)
		{
			SecondaryWep->bDestroyWeapon = true;
			Combat->EquipWeapon(SecondaryWep);
		}
	}
	// 3) Third
	if (ThirdWeaponClass && Combat)
	{
		AWeapon* ThirdWep = World->SpawnActor<AWeapon>(ThirdWeaponClass);
		if (ThirdWep)
		{
			ThirdWep->bDestroyWeapon = true;
			Combat->EquipThirdWeapon(ThirdWep);
		}
	}
}

void APlayerCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void APlayerCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		if (Combat->ThirdWeapon)
		{
			DropOrDestroyWeapon(Combat->ThirdWeapon);
		}
	}
}
#pragma endregion

#pragma region HUD Updates

void APlayerCharacter::UpdateHUDHealth()
{
	NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void APlayerCharacter::UpdateHUDShield()
{
	NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void APlayerCharacter::UpdateHUDAmmo()
{
	if (!Combat || !Combat->EquippedWeapon) return;

	if (NecroSyntexPlayerController == nullptr)
	{
		NecroSyntexPlayerController = Cast<ANecroSyntexPlayerController>(Controller);
	}

	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		NecroSyntexPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
		bInitializeAmmo = false;
	}
	else
	{
		InitialCarriedAmmo = Combat->CarriedAmmo;
		InitialWeaponAmmo = Combat->EquippedWeapon->GetAmmo();
		bInitializeAmmo = true;
	}
}
#pragma endregion

#pragma region Aim Offset & Rotation
void APlayerCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
	AO_Pitch = GetBaseAimRotation().Pitch;
}

void APlayerCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void APlayerCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}
void APlayerCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void APlayerCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

float APlayerCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}
#pragma endregion

#pragma region Doping System
void APlayerCharacter::FirstDoping()
{
	if (UDC)
	{
	
		UDC->PressedFirstDopingKey();
	}
}

void APlayerCharacter::SecondDoping()
{
	if (UDC)
	{
		UDC->PressedSecondDopingKey();
	}
}

void APlayerCharacter::DopingModeChange()
{
	if (UDC)
	{
		UDC->DopingModeChange();
	}
}


UDopingComponent* APlayerCharacter::GetDopingComponent()
{
	return UDC;
}

#pragma endregion

#pragma region Camera Effects & HeadBob

void APlayerCharacter::HandleHeadBob(float DeltaTime)
{
	if (!IsLocallyControlled()) return;

	if (!NecroSyntexPlayerController || !NecroSyntexPlayerController->PlayerCameraManager)
	{
		NecroSyntexPlayerController = Cast<ANecroSyntexPlayerController>(Controller);
		if (!NecroSyntexPlayerController || !NecroSyntexPlayerController->PlayerCameraManager) return;
	}

	const FVector HorizontalVelocity(GetVelocity().X, GetVelocity().Y, 0.f);
	const float Speed = HorizontalVelocity.Size();

	TSubclassOf<UCameraShakeBase> DesiredShake = nullptr;
	if (Speed > WalkSpeed && bIsSprinting)
	{
		DesiredShake = SprintHeadBob;
	}
	else if (Speed > 0.1f)
	{
		DesiredShake = WalkHeadBob;
	}
	else
	{
		DesiredShake = IdleHeadBob;
	}

	if (DesiredShake)
	{
		NecroSyntexPlayerController->PlayerCameraManager->StartCameraShake(
			DesiredShake,
			1.0f,
			ECameraShakePlaySpace::CameraLocal,
			FRotator::ZeroRotator
		);
	}
}

void APlayerCharacter::HSDeBuffON()
{
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(UDeBuffCameraShake::StaticClass());
}

void APlayerCharacter::SPStrengthDeBuffON()
{

	// 블러 효과 추가
	FollowCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	FollowCamera->PostProcessSettings.MotionBlurAmount = 1.0f; // 블러 강도 (0.0 ~ 1.0)

	// 색수차(번짐) 효과 추가
	FollowCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 8.0f; // 색수차 강도

	// 디버프 활성화 (BlendWeight 조절)
	FollowCamera->PostProcessBlendWeight = 1.0f;
}

void APlayerCharacter::SPStrengthDeBuffOFF()
{
	FollowCamera->PostProcessBlendWeight = 0.0f;
}

#pragma endregion

#pragma region Replication & Network
void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME(APlayerCharacter, bDisableGameplay);
	DOREPLIFETIME(APlayerCharacter, Health);
	DOREPLIFETIME(APlayerCharacter, Shield);
	DOREPLIFETIME(APlayerCharacter, HealingStationActor);
	DOREPLIFETIME(APlayerCharacter, OverlappingSupplyCrate);

	DOREPLIFETIME(APlayerCharacter, WalkSpeed);
	DOREPLIFETIME(APlayerCharacter, RunningSpeed);
	DOREPLIFETIME(APlayerCharacter, MLAtaackPoint);
	DOREPLIFETIME(APlayerCharacter, Defense);
	DOREPLIFETIME(APlayerCharacter, Blurred);
	DOREPLIFETIME(APlayerCharacter, ROF);
	DOREPLIFETIME(APlayerCharacter, DopingDamageBuff);
	DOREPLIFETIME(APlayerCharacter, ReservedMoving);

}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void APlayerCharacter::OnRep_bIsSprinting()
{
	if (!IsLocallyControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed =
			bIsSprinting ? RunningSpeed : WalkSpeed;
	}
}

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void APlayerCharacter::OnRep_Health(float LastHealth)
{

	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayerHitReactMontage();
	}
}

void APlayerCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
}
#pragma endregion

#pragma region Utility & Helpers
void APlayerCharacter::PollInit()
{
	if (NecroSyntexPlayerState == nullptr)
	{
		NecroSyntexPlayerState = GetPlayerState<ANecroSyntexPlayerState>();
		if (NecroSyntexPlayerState)
		{
			NecroSyntexPlayerState->AddToScore(0.f);
			NecroSyntexPlayerState->AddToDefeats(0);
		}
	}
}

void APlayerCharacter::SetHealingStationActor(AHealingStation* Station)
{
	HealingStationActor = Station;
}

void APlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void APlayerCharacter::SetOverlappingSupplyCrate(ASupplyCrate* Crate)
{
	OverlappingSupplyCrate = Crate;
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

void APlayerCharacter::Destroyed()
{
	Super::Destroyed();

	ANecroSyntexGameMode* NecroSyntexGameMode = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = NecroSyntexGameMode && NecroSyntexGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void APlayerCharacter::ActivateDissolveEffect()
{
	if (DissolveEffectComponent && !DissolveEffectComponent->IsActive())
	{
		DissolveEffectComponent->Activate(true);
	}
}

ECombatState APlayerCharacter::GetCombatState() const
{
	if (Combat == nullptr)
	{
		return ECombatState::ECS_MAX;
	}
	else
	{
		return Combat->CombatState;
	}
}

AWeapon* APlayerCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector APlayerCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

void APlayerCharacter::ServerRequestHealing_Implementation()
{
	if (HealingStationActor)
	{
		HealingStationActor->Interact(this);
	}
}
#pragma endregion
