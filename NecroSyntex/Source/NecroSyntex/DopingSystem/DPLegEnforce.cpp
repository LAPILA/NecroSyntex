#include "DPLegEnforce.h"

UDPLegEnforce::UDPLegEnforce()
	: Super()
{
	BuffDuration = 5.0f;
	DeBuffDuration = 2.0f;

	WalkingBuffNum = 350.0f;
	RunningBuffNum = 1000.0f;
	WalkingDeBuffNum = 150.0f;
	RunningDeBuffNum = 300.0f;
}

void UDPLegEnforce::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;

		// Buff 적용 및 타이머 시작
		BuffOn(PID);

		// 쿨타임 시작
		StartCooldown();

		UE_LOG(LogTemp, Warning, TEXT("LegEnforce Use"));
	}
}

void UDPLegEnforce::BuffOn(UPlayerInformData* PID)
{
	PID->MoveSpeed += WalkingBuffNum;
	PID->RunningSpeed += RunningBuffNum;

	CheckBuff = true;

	PID->CurrentDoped += 1;

	StartBuff(PID);

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce BuffOn: Speed increased."));
}

void UDPLegEnforce::BuffOff(UPlayerInformData* PID)
{
	PID->MoveSpeed -= WalkingBuffNum;
	PID->RunningSpeed -= RunningBuffNum;

	CheckBuff = false;

	// DeBuff 적용 및 타이머 시작
	DeBuffOn(PID);

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce BuffOff: Speed normalized."));
}

void UDPLegEnforce::DeBuffOn(UPlayerInformData* PID)
{
	PID->MoveSpeed -= WalkingDeBuffNum;
	PID->RunningSpeed -= RunningDeBuffNum;

	CheckDeBuff = true;

	StartDeBuff(PID);

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce DeBuffOn: Speed reduced."));
}

void UDPLegEnforce::DeBuffOff(UPlayerInformData* PID)
{
	PID->MoveSpeed += WalkingDeBuffNum;
	PID->RunningSpeed += RunningDeBuffNum;

	PID->CurrentDoped -= 1;

	CheckDeBuff = false;

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce DeBuffOff: Speed restored."));
}
