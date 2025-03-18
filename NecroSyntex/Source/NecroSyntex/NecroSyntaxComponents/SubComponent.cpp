
#include "SubComponent.h"
#include "NecroSyntex\Character\PlayerCharacter.h"

USubComponent::USubComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USubComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<APlayerCharacter>(GetOwner());
	LastDamageTime = GetWorld()->GetTimeSeconds() - ShieldRegenDelay;
}

void USubComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}


void USubComponent::Heal(float HealAmount, float HealingTime)
{
	if (!Character || Character->IsElimed()) return;

	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void USubComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || !Character || Character->IsElimed()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	float NewHealth = FMath::Clamp(
		Character->GetHealth() + HealThisFrame,
		0.f,
		Character->GetMaxHealth()
	);

	Character->SetHealth(NewHealth);
	Character->UpdateHUDHealth();

	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void USubComponent::OnTakeDamage()
{
	LastDamageTime = GetWorld()->GetTimeSeconds();
}

void USubComponent::ShieldRampUp(float DeltaTime)
{
	if (!Character || Character->IsElimed()) return;

	float CurrentShield = Character->GetShield();
	if (CurrentShield >= Character->GetMaxShield()) return;

	float TimeSinceDamage = GetWorld()->GetTimeSeconds() - LastDamageTime;
	if (TimeSinceDamage < ShieldRegenDelay)
	{
		return;
	}

	float ShieldThisFrame = ShieldRegenRate * DeltaTime;
	float NewShield = FMath::Clamp(
		CurrentShield + ShieldThisFrame,
		0.f,
		Character->GetMaxShield()
	);

	Character->SetShield(NewShield);
	Character->UpdateHUDShield();
}