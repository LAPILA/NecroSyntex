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
#include "NecroSyntex\NecroSyntaxComponents\LagCompensationComponent.h"

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
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	back = CreateDefaultSubobject<UBoxComponent>(TEXT("back"));
	back->SetupAttachment(GetMesh(), FName("neck_01"));
	back->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("back"), back);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);
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
	// GameMode�� World ��ȿ�� �˻�
	ANecroSyntexGameMode* NecroSyntexGameMode = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (!NecroSyntexGameMode || !World || bElimed) return;

	// 1) �ֹ���(Primary) ����
	if (DefaultWeaponClass && Combat)
	{
		AWeapon* PrimaryWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		if (PrimaryWeapon)
		{
			// bDestroyWeapon = true �� ��� �� �ı��Ǵ� ����
			PrimaryWeapon->bDestroyWeapon = true;
			Combat->EquipWeapon(PrimaryWeapon);
			// -> Combat->EquipWeapon() ���ο��� 
			//    '���� �̹� ���Ⱑ ������ SecondaryWeapon���� ����' ���� ó��
		}
	}

	// 2) ���� ����(Secondary) ����
	if (SubWeaponClass && Combat)
	{
		AWeapon* SecondaryWeapon = World->SpawnActor<AWeapon>(SubWeaponClass);
		if (SecondaryWeapon)
		{
			SecondaryWeapon->bDestroyWeapon = true;
			Combat->EquipWeapon(SecondaryWeapon);
			// -> ù ��° ���Ⱑ �ֹ��Ⱑ �Ǿ����Ƿ�, �� ��°�� ���� ����� �ڵ� Equip
		}
	}

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
	UpdateHUDAmmo();
	UpdateHUDShield();
	UpdateHUDHealth();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
	if (!UDC) {
		GetCharacterMovement()->MaxWalkSpeed = 550.0f;
	}
	else {
		if (HasAuthority()) {
			GetCharacterMovement()->MaxWalkSpeed = UDC->PID->MoveSpeed;
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

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
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(ThrowGrenade, ETriggerEvent::Triggered, this, &APlayerCharacter::GrenadeButtonPressed);
		EnhancedInputComponent->BindAction(SwapWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapWeaponWheel);

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
	DOREPLIFETIME(APlayerCharacter, bDisableGameplay);
	DOREPLIFETIME(APlayerCharacter, Health);
	DOREPLIFETIME(APlayerCharacter, Shield);
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

		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);
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

	if (HasAuthority()) {
		if (UDC) {
			if (UDC->CurseofChaos) {
				if (UDC->CurseofChaos->CheckDeBuff == true)
				{
					MovementVector.X *= -1;
					MovementVector.Y *= -1;
				}
			}
		}
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
}

void APlayerCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
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
			GetCharacterMovement()->MaxWalkSpeed = UDC->PID->RunningSpeed;
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
	if (bIsSprinting)
	{
		bIsSprinting = false;

		if (HasAuthority())
		{
			GetCharacterMovement()->MaxWalkSpeed = UDC->PID->MoveSpeed;
		}
		else
		{
			ServerSprintStop();
		}
	}
}

void APlayerCharacter::ServerSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = UDC->PID->RunningSpeed;
}

void APlayerCharacter::ServerSprintStop_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = UDC->PID->MoveSpeed;
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
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(false);
	}
}

void APlayerCharacter::FlashButtonPressed()
{
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
	if (HasAuthority())
	{
		if (Combat && Combat->ShouldSwapWeapons())
		{
			Combat->SwapWeapons();
		}
	}
	else
	{
		ServerSwapWeaponWheel();
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
	}
}

bool APlayerCharacter::ServerSwapWeaponWheel_Validate()
{
	return true;
}

void APlayerCharacter::ServerSwapWeaponWheel_Implementation()
{
	if (Combat && Combat->ShouldSwapWeapons())
	{
		Combat->SwapWeapons();
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
	NecroSyntexPlayerController = NecroSyntexPlayerController == nullptr ? Cast<ANecroSyntexPlayerController>(Controller) : NecroSyntexPlayerController;
	if (NecroSyntexPlayerController && Combat && Combat->EquippedWeapon)
	{
		NecroSyntexPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		NecroSyntexPlayerController->SetHUDCarriedAmmo(Combat->EquippedWeapon->GetAmmo());
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

	// Checking if we got an interruption on the montage. I get interruption on high ping even if there is nothing overriding it, so the animation will still play
	if (bInterrupted)
	{
		// The current time on the reload montage
		float CurrentPosition = AnimInstance->Montage_GetPosition(ReloadMontage);

		FName SectionName = AnimInstance->Montage_GetCurrentSection();

		int32 SectionIndex = ReloadMontage->GetSectionIndex(SectionName);

		// From OutStart and OutEndTime we can know the length of the section, we do it so it will work with every reload animation runtime, to prevent going into our reload montage and store all durations in a map or similar.
		float OutStartTime;
		float OutEndTime;
		ReloadMontage->GetSectionStartAndEndTime(SectionIndex, OutStartTime, OutEndTime);

		// How long we need to wait depending on where the montage was interrupted
		float TimeToWait = FMath::Clamp(OutEndTime - CurrentPosition, 0, OutEndTime - OutStartTime);


		// If we get interrupted at the end of our reload sequence, there is no need to start a timer if TimeToWait is 0, otherwise we start a wait timer.
		if (TimeToWait > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				ReloadTimer,
				this,
				&APlayerCharacter::ReloadTimerFinished,
				TimeToWait);
		}
		// We need special case if we are on a shotgun, we want to be able to shoot during reload
		// Then 
		else if (Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
		{
			// JumpToShotgunEnd got an update too to set state Unoccupied if we are on server (posted below)
			Combat->JumpToShotgunEnd();
		}

		// Unbind ReloadMontageEndedHandler
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::ReloadMontageEndedHandler);

	}
	else
	{
		// If we didn't get an interruption, then everything is fine and we just call finish reloading directly
		Combat->FinishReloading();
		// Unbind ReloadMontageEndedHandler
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


//Pahu
float APlayerCharacter::GetTotalDamage()
{
	TotalDamage = UDC->TotalDamage;

	return TotalDamage;

}


UDopingComponent* APlayerCharacter::GetDopingComponent()
{
	return UDC;
}

void APlayerCharacter::GetDopingFromAlly()
{

}