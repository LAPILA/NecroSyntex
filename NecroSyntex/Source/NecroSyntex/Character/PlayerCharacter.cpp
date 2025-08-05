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
#include "NecroSyntex/NecroSyntaxComponents/DR_FlashDroneComponent.h"
#include "NecroSyntex/NecroSyntaxComponents/DR_FlashDrone.h"

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

	FlashDroneComponent = CreateDefaultSubobject<UDR_FlashDroneComponent>(TEXT("FlashDroneComp"));

	bReplicates = true;
	SetReplicateMovement(true);
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
		Health = MaxHealth;
		Shield = MaxShield;
	}

	if (HasAuthority() && FlashDroneComponent)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
	}
}
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ANecroSyntexPlayerController* NPC = Cast<ANecroSyntexPlayerController>(NewController);
	if (!NPC) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("NecroSyntex Not Possessd XXXXXX"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("NecroSyntex Possessd OOOOOOOO"));
		UE_LOG(LogTemp, Error, TEXT("Possessed 작동"));
	}

	if (UDC) {
		UDC->InitDopingSkillSet();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("UDC 인식 안됨"));
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

	if (IsLocallyControlled() && FlashDroneComponent)
	{
		if (ADR_FlashDrone* Drone = FlashDroneComponent->GetFlashDrone())
		{
			const FVector AimTarget = (Combat && Combat->bAiming) ? Combat->HitTarget
				: FVector::ZeroVector;
			Drone->SetAimTarget(AimTarget);
		}
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
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
		EnhancedInputComponent->BindAction(ThrowGrenade, ETriggerEvent::Started, this, &APlayerCharacter::GrenadeButtonPressed);
		EnhancedInputComponent->BindAction(ThrowGrenade, ETriggerEvent::Completed, this, &APlayerCharacter::ResetGrenadeState);
		EnhancedInputComponent->BindAction(SwapWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapWeaponWheel);
		EnhancedInputComponent->BindAction(UDCskill1, ETriggerEvent::Triggered, this, &APlayerCharacter::FirstDoping);
		EnhancedInputComponent->BindAction(UDCskill2, ETriggerEvent::Triggered, this, &APlayerCharacter::SecondDoping);
		EnhancedInputComponent->BindAction(UDCModeChange, ETriggerEvent::Triggered, this, &APlayerCharacter::DopingModeChange);
		EnhancedInputComponent->BindAction(SwapFirstWeapon, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapToFirstWeapon);
		EnhancedInputComponent->BindAction(SwapSecondWeapon, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapToSecondWeapon);
		EnhancedInputComponent->BindAction(SwapThirdWeapon, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapToThirdWeapon);
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input success"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input fail"));
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

		// 클라는 AddMovementInput만.
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	FVector2D LookInput = Value.Get<FVector2D>();

	// 서버에 회전 입력 전송 (본인 컨트롤일 때만)
	if (Controller && IsLocallyControlled())
	{
		if (!HasAuthority()) {
			ServerUpdateLook(LookInput.X, LookInput.Y);
		}
		//ServerUpdateLook(LookInput.X, LookInput.Y);
		
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void APlayerCharacter::ServerUpdateLook_Implementation(float YawInput, float PitchInput)
{
	if (Controller)
	{
		FRotator NewRotation = Controller->GetControlRotation();
		NewRotation.Yaw += YawInput;
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + PitchInput, -89.f, 89.f);
		Controller->SetControlRotation(NewRotation);

		// **핵심: RootComponent(캡슐)의 회전도 변경해야 복제된다**
		SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
	}
}
bool APlayerCharacter::ServerUpdateLook_Validate(float, float) { return true; }

void APlayerCharacter::SprintStart()
{
	if (bDisableGameplay) return;

	bWantsToSprint = true;
	if (!HasAuthority())
	{
		// 클라 예측 반영
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		ServerSprintStart();
		return;
	}

	if (!bIsSprinting && GetVelocity().Size() > 0.f)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}
void APlayerCharacter::SprintStop()
{
	if (bDisableGameplay) return;

	bWantsToSprint = false;
	if (!HasAuthority())
	{
		// 클라 예측 반영
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		ServerSprintStop();
		return;
	}

	if (bIsSprinting)
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}


void APlayerCharacter::ServerSprintStart_Implementation()
{
	SprintStart();
}
void APlayerCharacter::ServerSprintStop_Implementation()
{
	SprintStop();
}
bool APlayerCharacter::ServerSprintStart_Validate() { return true; }
bool APlayerCharacter::ServerSprintStop_Validate() { return true; }


void APlayerCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat->CombatState == ECombatState::ECS_ThrowingGrenade) return;

	if (!HasAuthority())
	{
		if (bIsCrouched) UnCrouch();
		else Crouch();
		ServerCrouchButtonPressed();
		return;
	}

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

void APlayerCharacter::ServerCrouchButtonPressed_Implementation()
{
	CrouchButtonPressed();
}
bool APlayerCharacter::ServerCrouchButtonPressed_Validate() { return true; }

#pragma endregion

#pragma region Combat Actions
void APlayerCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (bElimed || bDisableGameplay) return;

	if (Combat && Combat->CombatState == ECombatState::ECS_ThrowingGrenade) return;

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (bElimed || bDisableGameplay) return;

	if (Combat && Combat->CombatState == ECombatState::ECS_ThrowingGrenade) return;

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
	if (bDisableGameplay) return;
	if (bIsMontagePlaying || Combat == nullptr) return;

	// 이미 던지는 중이면 무시
	if (Combat->CombatState == ECombatState::ECS_ThrowingGrenade)
	{
		return;
	}

	// 상태 설정 및 몽타주 타이머 시작
	Combat->CombatState = ECombatState::ECS_ThrowingGrenade;
	SetMontagePlaying(true);

	// 실제 수류탄 투척 함수 실행
	Combat->ThrowGrenade();
}

void APlayerCharacter::ResetGrenadeState()
{
	if (Combat)
	{
		Combat->CombatState = ECombatState::ECS_Unoccupied;
	}
	SetMontagePlaying(false);
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
	if (bDisableGameplay) return;
	if (Combat)
	{
		PlaySwapMontage();
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
		PlaySwapMontage();
	}
}

void APlayerCharacter::SwapToFirstWeapon()
{
	if (bDisableGameplay) return;
	if (CanSwapWeapon())
	{
		Combat->SwapWeaponByNumber(1);
		StartWeaponSwapCooldown();
	}
}

void APlayerCharacter::SwapToSecondWeapon()
{
	if (bDisableGameplay) return;
	if (CanSwapWeapon())
	{
		Combat->SwapWeaponByNumber(2);
		StartWeaponSwapCooldown();
	}
}

void APlayerCharacter::SwapToThirdWeapon()
{
	if (bDisableGameplay) return;
	if (CanSwapWeapon())
	{
		Combat->SwapWeaponByNumber(3);
		StartWeaponSwapCooldown();
	}
}

bool APlayerCharacter::CanSwapWeapon() const
{
	return (Combat && !bDisableGameplay && Combat->bCanSwapWeapon);
}

void APlayerCharacter::StartWeaponSwapCooldown()
{
	Combat->bCanSwapWeapon = false;
	GetWorld()->GetTimerManager().SetTimer(
		Combat->SwapCooldownTimer,
		this,
		&APlayerCharacter::ResetWeaponSwapCooldown,
		Combat->SwapCooldownTime,
		false
	);
}

void APlayerCharacter::ResetWeaponSwapCooldown()
{
	Combat->bCanSwapWeapon = true;
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
	if (!FlashDroneComponent) return;

	if (ADR_FlashDrone* Drone = FlashDroneComponent->GetFlashDrone())
	{
		bFlashLightOn = !bFlashLightOn;          // 토글
		Drone->ToggleFlash(bFlashLightOn);       // 앞서 만든 RPC 호출
	}
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

	if (IsLocallyControlled() && NecroSyntexPlayerController && NecroSyntexPlayerController->PlayerCameraManager)
	{
		if (WalkHeadBob)
		{
			NecroSyntexPlayerController->PlayerCameraManager->StartCameraShake(
				WalkHeadBob,
				1.0f,
				ECameraShakePlaySpace::CameraLocal,
				FRotator::ZeroRotator
			);
		}
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
		// 델리게이트 바인딩: 종료 시 호출
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnThrowGrenadeMontageEnded);
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::OnThrowGrenadeMontageEnded);

		// 몽타주 재생
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void APlayerCharacter::OnThrowGrenadeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 끝났을 때 처리
	if (Montage == ThrowGrenadeMontage)
	{
		if (Combat)
		{
			Combat->ThrowGrenadeFinished();
		}

		// 델리게이트 제거
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnThrowGrenadeMontageEnded);
		}
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

void APlayerCharacter::SetMontagePlaying(bool bIsPlaying)
{
	bIsMontagePlaying = bIsPlaying;
}

void APlayerCharacter::ResetMontageState()
{
	SetMontagePlaying(false);
	if (Combat)
	{
		Combat->CombatState = ECombatState::ECS_Unoccupied;
		Combat->bCanFire = true;

		// 무기 상태 복구
		if (Combat->EquippedWeapon)
		{
			Combat->AttachActorToRightHand(Combat->EquippedWeapon);
		}

		// 모든 상태 초기화
		bIsSprinting = false;
		bWantsToSprint = false;
		if (bIsCrouched)
		{
			UnCrouch();
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = Combat->BaseWalkSpeed;
		}
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
	TakeDamageNotify(Damage);
	PlayerHitReactMontage();

	float NewDamage = Damage - Defense;
	if (NewDamage <= 0.0f) {
		NewDamage = 0.0f;
	}

	if (GEngine)
	{
		FString DamageText = FString::Printf(TEXT("Received : %f"), NewDamage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DamageText);
	}

	if (Combat && Combat->CombatState == ECombatState::ECS_Reloading)
	{
		Combat->CancelReload();
	}

	if (Shield > 0)
	{
		float NewShieldValue = FMath::Clamp(Shield - NewDamage, 0.f, MaxShield);

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
		Health = FMath::Clamp(Health - NewDamage, 0.f, MaxHealth);
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

void APlayerCharacter::ClientUpdateHUDHealth_Implementation()
{
	UpdateHUDHealth();
}

void APlayerCharacter::UpdateHUDShield()
{
	NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void APlayerCharacter::ClientUpdateHUDShield_Implementation()
{
	UpdateHUDShield();
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
	if (bDisableGameplay) return;
	if (bIsMontagePlaying || Combat->CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	if (UDC)
	{
		SetMontagePlaying(true);
		UDC->PressedFirstDopingKey();
		GetWorldTimerManager().SetTimer(MontageEndTimer, this, &APlayerCharacter::ResetMontageState, 0.5f, false);
	}
}

void APlayerCharacter::SecondDoping()
{
	if (bDisableGameplay) return;
	if (bIsMontagePlaying || Combat->CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	if (UDC)
	{
		SetMontagePlaying(true);
		UDC->PressedSecondDopingKey();
		GetWorldTimerManager().SetTimer(MontageEndTimer, this, &APlayerCharacter::ResetMontageState, 0.5f, false);
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
	if (bElimed) return;

	static float AimStartTime = 0.f;
	static bool bWasAiming = false;
	bool bNowAiming = IsAiming();

	if (bNowAiming && !bWasAiming)
	{
		AimStartTime = GetWorld()->GetTimeSeconds();
	}
	bWasAiming = bNowAiming;

	if (bNowAiming)
	{
		if (GetWorld()->GetTimeSeconds() - AimStartTime < 3.f)
			return; // 조준 시작 후 3초까지는 헤드밥 억제
	}

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
	if (ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(GetController()))
	{
		PC->ClientStartCameraShake(UDeBuffCameraShake::StaticClass());
	}
}

void APlayerCharacter::SPStrengthDeBuffON_Implementation()
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

void APlayerCharacter::SPStrengthDeBuffOFF_Implementation()
{
	FollowCamera->PostProcessBlendWeight = 1.0f;

	// 블러 효과 추가
	FollowCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	FollowCamera->PostProcessSettings.MotionBlurAmount = 0.0f; // 블러 강도 (0.0 ~ 1.0)

	// 색수차(번짐) 효과 추가
	FollowCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 0.0f; // 색수차 강도
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
	DOREPLIFETIME(APlayerCharacter, MaxHealth);
	DOREPLIFETIME(APlayerCharacter, Shield);
	DOREPLIFETIME(APlayerCharacter, MaxShield);
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

void APlayerCharacter::OnRep_IsSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? RunningSpeed : WalkSpeed;
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

void APlayerCharacter::OnRep_MaxHealth()
{
	UpdateHUDHealth();
}

void APlayerCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
}
void APlayerCharacter::OnRep_MaxShield()
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

void APlayerCharacter::UseGrenade()
{
	Combat->RemoveGrenade(1);
}

int32 APlayerCharacter::GetCurrentGrenadeCount() const
{
	if (Combat)
	{
		return Combat->GetGrenades();
	}
	return 0;
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
