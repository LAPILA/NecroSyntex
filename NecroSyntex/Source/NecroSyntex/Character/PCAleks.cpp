// Fill out your copyright notice in the Description page of Project Settings.


#include "PCAleks.h"
#include "NecroSyntex/DopingSystem/DCAleks.h"

APCAleks::APCAleks()
{
	UDC = CreateDefaultSubobject<UDCAleks>(TEXT("DopingComponent"));
}