// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NecroSyntex/NecroSyntexType/TurningInPlace.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NecroSyntex/Interfaces/InteractWithCrossHairsInterface.h"
#include "NecroSyntex/NecroSyntexType/CombatState.h"
#include "NecroSyntex/Voice/VoiceComponent.h"
#include "NecroSyntex/NecroSyntaxComponents/DR_FlashDroneComponent.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class NECROSYNTEX_API APlayerCharacter : public ACharacter, public IInteractWithCrossHairsInterface
{
	GENERATED_BODY()

public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
private:
	/** Key Settings */
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowGrenade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UDCskill1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UDCskill2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UDCModeChange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapFirstWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapSecondWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapThirdWeapon;
public:
    APlayerCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	/*
		Montage
	*/
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	UFUNCTION()
	void OnThrowGrenadeMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PlaySwapMontage();
	void PlayDopingMontage();
	virtual void OnRep_ReplicatedMovement() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = true;

	UFUNCTION(BluePrintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void TakeDamageNotify(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void UpdateHUDHealth();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientUpdateHUDHealth();

	UFUNCTION(BlueprintCallable)
	void UpdateHUDShield();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientUpdateHUDShield();

	bool bInitializeAmmo = false;
	int32 InitialCarriedAmmo = 0;
	int32 InitialWeaponAmmo = 0;


	void UpdateHUDAmmo();

	UFUNCTION(BlueprintCallable)
	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;


	bool bFinishedSwapping = false;

	UPROPERTY(Replicated)
	class AHealingStation* HealingStationActor = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetHealingStationActor(AHealingStation* Station);

	UPROPERTY(Replicated)
	class ASupplyCrate* OverlappingSupplyCrate = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetOverlappingSupplyCrate(ASupplyCrate* Crate);

	UFUNCTION(Server, Reliable)
	void ServerRequestHealing();
	void ServerRequestHealing_Implementation();
	bool ServerRequestHealing_Validate() { return true; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnWeaponHitEvent(const FHitResult& HitResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsMontagePlaying = false;

	FTimerHandle MontageEndTimer;

	void SetMontagePlaying(bool bIsPlaying);

	void ResetMontageState();

	UFUNCTION(BlueprintCallable) //protected->public changed by duream
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
protected:
    virtual void BeginPlay() override;

	//Key Settings
	void Move(const FInputActionValue& Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void SprintStart();
	void SprintStop();
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void FlashButtonPressed();
	void ReloadButtonPressed();
	void PlayerHitReactMontage();
	void GrenadeButtonPressed();
	void ResetGrenadeState();
	void SwapWeaponWheel();
	void FirstDoping();
	void SecondDoping();
	void DopingModeChange();
	void SwapToFirstWeapon();
	void SwapToSecondWeapon();
	void SwapToThirdWeapon();

	bool CanSwapWeapon() const;

	void StartWeaponSwapCooldown();

	void ResetWeaponSwapCooldown();

	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	
	void PollInit();
	void RotateInPlace(float DeltaTime);


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintStart();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintStop();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouchButtonPressed();

	/**
	* Hit boxes used for server-side rewind
	*/

	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* back;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting;

	bool bWantsToSprint = false;

private:
	UFUNCTION()
	void OnRep_IsSprinting();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/**
	* NecroSyntex components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USubComponent* SubComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UVoiceComponent* VoiceComp;

	UPROPERTY(EditDefaultsOnly, Category = "Voice")
	TObjectPtr<UVoiceSet> DefaultVoiceSet;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeaponWheel();

	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMongatge;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* DopingMontage;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/**
	* Player health
	*/
	
	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UFUNCTION()
	void OnRep_MaxHealth();
	/**
	* Player sheild
	*/
	
	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UFUNCTION()
	void OnRep_MaxShield();

	bool bElimed = false;

	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 10.f;
	void ElimTimerFinished();

	UPROPERTY()
	class ANecroSyntexPlayerController* NecroSyntexPlayerController;

	/**
	* Dissolve effect
	*/

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	UNiagaraComponent* DissolveEffectComponent;

	UFUNCTION()
	void ActivateDissolveEffect();

	class ANecroSyntexPlayerState* NecroSyntexPlayerState;

	/**
	* Grenade
	*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	/**
	* Default weapon
	*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;


	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> SubWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> ThirdWeaponClass;
	UFUNCTION()
	void ReloadMontageEndedHandler(UAnimMontage* Montage, bool bInterrupted);

	FTimerHandle ReloadTimer;

	UFUNCTION()
	void ReloadTimerFinished();

	// Headbob 관련 속성
	UPROPERTY(EditAnywhere, Category = "HeadBob")
	TSubclassOf<UCameraShakeBase> IdleHeadBob;

	UPROPERTY(EditAnywhere, Category = "HeadBob")
	TSubclassOf<UCameraShakeBase> WalkHeadBob;

	UPROPERTY(EditAnywhere, Category = "HeadBob")
	TSubclassOf<UCameraShakeBase> SprintHeadBob;

	void HandleHeadBob(float DeltaTime);

	UPROPERTY()
	bool bFlashLightOn = true;
public:
	//Pahu
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//Doping Component(by TeaHyuck)
	class UDopingComponent* UDC;

	UPROPERTY(EditDefaultsOnly, Category = "Doping")
	TSubclassOf<UDopingComponent> DopingComponentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDR_FlashDroneComponent* FlashDroneComponent;

	UFUNCTION(BlueprintCallable)
	UDopingComponent* GetDopingComponent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayDopingEffect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHUDFirstDopingTrueicon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHUDFirstDopingFalseicon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHUDSecondDopingTrueicon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHUDSecondDopingFalseicon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CallBlueprintBurningFurnaceDamage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayerDeathStopDopingEffect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDopingIconHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void SetHUDRemainFirstDoping();

	UFUNCTION(BlueprintImplementableEvent)
	void SetHUDRemainSecondDoping();

	UFUNCTION(NetMulticast, Reliable)
	void SPStrengthDeBuffON();

	UFUNCTION(NetMulticast, Reliable)
	void SPStrengthDeBuffOFF();

	UFUNCTION()
	void HSDeBuffON();


	//PID(Player Inform Data)
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadWrite, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_MaxShield, EditAnywhere, Category = "Player Stats")
	float MaxShield = 200.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Shield = 200.f;


	//Speed
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float RunningSpeed = 1200.f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float AimWalkSpeed = 400.f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float CrouchSpeed = 300.f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float ReloadSpeedMultiplier = 0.5f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float GrenadeThrowSpeedMultiplier = 0.5f;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Movement")
	float MaxCharacterSpeed = 10000.f;



	float Rebound; // 반동

	UPROPERTY(Replicated, EditAnywhere)
	float MLAtaackPoint; // 근접 공격력
	UPROPERTY(Replicated, EditAnywhere, Category = "Player Stats")
	float Defense; // 방어력
	UPROPERTY(Replicated, EditAnywhere, Category = "Player Stats")
	float Blurred; // 시야(화면 흐림도)
	UPROPERTY(Replicated, EditAnywhere)
	float ROF; // 총 연사속도
	//float Item_UseRate; // 아이템 사용비율

	UPROPERTY(Replicated, EditAnywhere, Category = "Player Stats")
	float DopingDamageBuff; // 도핑으로 강화된 공격력

	UPROPERTY(Replicated, EditAnywhere)
	bool ReservedMoving = false; //좌우 뒤바낌 움직임 여부

	/*float BaseMaxHealth;
	float BaseCurrentHealth;
	float BaseMaxShield;
	float BaseCurrentShield;
	float BaseAttackPointMag;
	float BaseMoveSpeed;
	float BaseRunningSpeed;
	float BaseRebound;
	float BaseMLAttackPoint;
	float BaseDefense;
	float BaseBlurred;
	float BaseROF;*/

	//현재 캐릭터가 걸려있는 도핑(디버프 상태 포함)
	UPROPERTY()
	int CurrentDoped;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UseGrenade();
	//Pahu end

	UFUNCTION(BlueprintCallable, Category = "Grenade")
	int32 GetCurrentGrenadeCount() const;

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimed() const { return bElimed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE USubComponent* GetSubComp() const { return SubComp; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE UVoiceComponent* GetVoiceComp() const { return VoiceComp; }
};