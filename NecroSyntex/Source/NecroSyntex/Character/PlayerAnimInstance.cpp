// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	//Check
	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}

	if (PlayerCharacter == nullptr) return;

	//playerinfo
	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	FRotator BaseRotation = TryGetPawnOwner()->GetActorRotation();

	// Calculate direction
	if (Speed > 0.0f)
	{
		Direction = CalculateDirection(Velocity, BaseRotation);
	}
	else
	{
		Direction = 0.0f;
	}

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
	bWeaponEquipped = PlayerCharacter->IsWeaponEquipped();
	bIsCrouch = PlayerCharacter->bIsCrouched;
	bAiming = PlayerCharacter->IsAiming();
}