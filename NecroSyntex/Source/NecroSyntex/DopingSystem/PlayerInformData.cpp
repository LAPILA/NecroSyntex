// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInformData.h"

UPlayerInformData::UPlayerInformData()
{
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	AttackPointMag = 1.0f;
	MoveSpeed = 200.0f;
	RunningSpeed = 600.0f;
	MLAtaackPoint = 10.0f;
	Defense = 10.0f;
	Blurred = 10.0f;
	ROF = 10.0f;
	RecoverAPS = 0.0f;

	BaseMaxHealth = 100.0f;
	BaseCurrentHealth = BaseMaxHealth;
	BaseAttackPointMag = 1.0f;
	BaseMoveSpeed = 200.0f;
	BaseRunningSpeed = 600.0f;
	//BaseRebound;
	BaseMLAttackPoint = 10.0f;
	BaseDefense = 10.0f;
	BaseBlurred = 10.0f;
	BaseROF = 10.0f;
}