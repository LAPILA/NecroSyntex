// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInformData.h"

UPlayerInformData::UPlayerInformData()
{
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	MoveSpeed = 550.0f;
	RunningSpeed = 1000.0f;
	MLAtaackPoint = 10.0f;
	Defense = 10.0f;
	Blurred = 10.0f;
	Rebound = 10.0f;
	ROF = 10.0f;

	DopingDamageBuff = 0.0f;

	BaseMaxHealth = 100.0f;
	BaseCurrentHealth = BaseMaxHealth;
	BaseAttackPointMag = 1.0f;
	BaseMoveSpeed = 200.0f;
	BaseRunningSpeed = 600.0f;
	BaseRebound = 10.0f;
	BaseMLAttackPoint = 10.0f;
	BaseDefense = 10.0f;
	BaseBlurred = 10.0f;
	BaseROF = 10.0f;

	CurrentDoped = 0;
}