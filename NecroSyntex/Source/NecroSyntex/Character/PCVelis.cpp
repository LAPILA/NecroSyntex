// Fill out your copyright notice in the Description page of Project Settings.


#include "PCVelis.h"
#include "NecroSyntex/DopingSystem/DCVelis.h"

APCVelis::APCVelis()
{
	UDC = CreateDefaultSubobject<UDCVelis>(TEXT("DopingComponent"));
}

