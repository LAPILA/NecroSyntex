// Unreal Engine �⺻ ���
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

// Enhanced Input ���� ���
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// ������Ʈ ���� ��� (NecroSyntex)
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
#include "NecroSyntex\NecroSyntaxComponents\LagCompensationComponent.h"
#include "NecroSyntex\PickUps\HealingStation.h"

// �ִϸ��̼� ���� ���
#include "PlayerAnimInstance.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	//Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 120.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	//UDC = CreateDefaultSubobject<UDopingComponent>(TEXT("DopingComponent"));

	SubComp = CreateDefaultSubobject<USubComponent>(TEXT("SubComponent"));
	SubComp->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	DissolveEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DissolveEffectComponent"));
	DissolveEffectComponent->SetupAttachment(GetMesh());

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/**
	* Hit boxes for server-side rewind
	*/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	back = CreateDefaultSubobject<UBoxComponent>(TEXT("back"));
	back->SetupAttachment(GetMesh(), FName("neck_01"));
	HitCollisionBoxes.Add(FName("back"), back);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	HealingStationActor = nullptr;

	// 기본 이동속도 설정
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->AirControl = 0.3f; // 공중에서 제어 가능성
	GetCharacterMovement()->MaxAcceleration = 4096.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
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
	// To Do: Nedd Check Rifle  
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


void APlayerCharacter::SetHealingStationActor(AHealingStation* Station)
{
	HealingStationActor = Station;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Key Mapping
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(DefaultMappingContext, 0);
	}
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

	if (!UDC) {
		UDC = NewObject<UDopingComponent>(this);
		UDC->RegisterComponent();
		AddInstanceComponent(UDC);
	}


	if (HasAuthority()) {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = GetCharacterMovement()->Velocity;
	if (CurrentVelocity.Size() > MaxCharacterSpeed)
	{
		GetCharacterMovement()->Velocity = CurrentVelocity.GetClampedToMaxSize(MaxCharacterSpeed);
	}
	RotateInPlace(DeltaTime);
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
	if (!bIsCrouched)
	{
		HandleHeadBob(DeltaTime);
	}

	PollInit();
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

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon,COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME(APlayerCharacter, bDisableGameplay);
	DOREPLIFETIME(APlayerCharacter, Health);
	DOREPLIFETIME(APlayerCharacter, Shield);
	DOREPLIFETIME(APlayerCharacter, HealingStationActor);

	DOREPLIFETIME(APlayerCharacter, WalkSpeed);
	DOREPLIFETIME(APlayerCharacter, RunningSpeed);
	DOREPLIFETIME(APlayerCharacter, MLAtaackPoint);
	DOREPLIFETIME(APlayerCharacter, Defense);
	DOREPLIFETIME(APlayerCharacter, Blurred);
	DOREPLIFETIME(APlayerCharacter, ROF);
	DOREPLIFETIME(APlayerCharacter, DopingDamageBuff);
	DOREPLIFETIME(APlayerCharacter, ReservedMoving);

}

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

void APlayerCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimed)
	{
		return;
	}

	PlayerHitReactMontage();

	if (Combat && Combat->CombatState == ECombatState::ECS_Reloading)
	{
		Combat->CancelReload();
	}

	if (Shield > 0)
	{
		float NewShieldValue = FMath::Clamp(Shield - Damage, 0.f, MaxShield);

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
		Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
		UpdateHUDHealth();

		if (Health == 0.0f)
		{
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

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (ReservedMoving) {
		MovementVector.X *= -1;
		MovementVector.Y *= -1;
	}


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		ServerEquipButtonPressed();
	}
	if (HealingStationActor)
	{
		ServerRequestHealing();
	}
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

void APlayerCharacter::SprintStart()
{
	if (bDisableGameplay) return;

	if (!bIsSprinting)
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

		GetCharacterMovement()->Velocity = GetVelocity().GetClampedToMaxSize(RunningSpeed);
	}
}

void APlayerCharacter::SprintStop()
{
	if (bDisableGameplay) return;
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

void APlayerCharacter::FlashButtonPressed()
{
}

void APlayerCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
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

void APlayerCharacter::FirstDoping()
{
	if (UDC)
	{
		PlayDopingMontage();
		UDC->PressedFirstDopingKey();
	}
}

void APlayerCharacter::SecondDoping()
{
	if (UDC)
	{
		PlayDopingMontage();
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

float APlayerCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

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

void APlayerCharacter::OnRep_Health(float LastHealth)
{

	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayerHitReactMontage();
	}
}

void APlayerCharacter::UpdateHUDHealth()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_OneAble Call"));

	NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
	if (NecroSyntexPlayerController)
	{
		NecroSyntexPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void APlayerCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
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

void APlayerCharacter::ActivateDissolveEffect()
{
	if (DissolveEffectComponent && !DissolveEffectComponent->IsActive())
	{
		DissolveEffectComponent->Activate(true);
	}
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

bool APlayerCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* APlayerCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
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

FVector APlayerCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
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

bool APlayerCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
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


void APlayerCharacter::ReloadTimerFinished()
{
	if (Combat)
	{
		Combat->FinishReloading();
	}
}

void APlayerCharacter::ServerRequestHealing_Implementation()
{
	if (HealingStationActor)
	{
		HealingStationActor->Interact(this);
	}
}

//Pahu
float APlayerCharacter::GetTotalDamage()
{
	TotalDamage = UDC->TotalDamage;

	return TotalDamage;

}

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

UDopingComponent* APlayerCharacter::GetDopingComponent()
{
	return UDC;
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

void APlayerCharacter::HSDeBuffON()
{
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(UDeBuffCameraShake::StaticClass());
}