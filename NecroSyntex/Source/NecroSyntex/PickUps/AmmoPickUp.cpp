// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickUp.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\NecroSyntaxComponents\CombatComponent.h"


void AAmmoPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBoxIndex, bool bFromSweap, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBoxIndex, bFromSweap, SweepResult);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UCombatComponent* Combat = PlayerCharacter->GetCombat();
		if (Combat) 
		{
			Combat->PickUpAmmo(WeaponType,AmmoAmount);
		}
	}

	Destroyed();
}
