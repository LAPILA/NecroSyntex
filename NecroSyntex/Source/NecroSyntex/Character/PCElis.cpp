// Fill out your copyright notice in the Description page of Project Settings.


#include "PCElis.h"
#include "NecroSyntex/DopingSystem/DCElis.h"

APCElis::APCElis()
{
	UDC = CreateDefaultSubobject<UDCElis>(TEXT("DopingComponent"));
}
