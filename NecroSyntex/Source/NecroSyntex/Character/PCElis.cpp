// Fill out your copyright notice in the Description page of Project Settings.


#include "PCElis.h"
#include "NecroSyntex/DopingSystem/DCElis.h"

APCElis::APCElis()
{
	UDC = CreateDefaultSubobject<UDCElis>(TEXT("DopingComponent"));
}


void APCElis::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MaxHealth = MaxHealth + 100.0f;
		Health = MaxHealth;
		MaxShield = MaxShield + 100.0f;
		Shield = MaxShield;
		//WalkSpeed = 300.0f;
		//RunningSpeed = 500.0f;
		MLAtaackPoint = 20.0f;
		Defense = 5.0f;
	}
}