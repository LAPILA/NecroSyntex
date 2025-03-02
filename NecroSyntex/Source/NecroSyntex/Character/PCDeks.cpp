// Fill out your copyright notice in the Description page of Project Settings.


#include "PCDeks.h"
#include "NecroSyntex/DopingSystem/DCDeks.h"

APCDeks::APCDeks()
{
	UDC = CreateDefaultSubobject<UDCDeks>(TEXT("DopingComponent"));
}