// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickUp.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\NecroSyntaxComponents\SubComponent.h"

AHealthPickUp::AHealthPickUp()
{
	bReplicates = true;

}

void AHealthPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBoxIndex, bool bFromSweap, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBoxIndex, bFromSweap, SweepResult);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		USubComponent* SubComp = PlayerCharacter->GetSubComp();
		if (SubComp)
		{
			SubComp->Heal(HealAmount, HealingTime);
		}
	}

	Destroyed();
}

void AHealthPickUp::Destroyed()
{

	Super::Destroyed();
}