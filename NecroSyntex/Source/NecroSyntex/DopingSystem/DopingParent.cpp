#include "DopingParent.h"

UDopingParent::UDopingParent()
{
	DopingItemNum = 3;
	DopingCoolTime = 10.0f;
	Able = true;

	BuffDuration = 5.0f;

	DeBuffDuration = 3.0f;

	CheckBuff = false;
	CheckDeBuff = false;
}

void UDopingParent::UseDopingItem(UPlayerInformData* PID)
{
	if (Able && DopingItemNum > 0)
	{
		--DopingItemNum;
		Able = false;

		// Buff 시작
		BuffOn(PID);
		StartBuff(PID);

		// 쿨타임 시작
		StartCooldown();
	}
}

void UDopingParent::BuffOn(UPlayerInformData* PID)
{
	// 기본 버프 로직 (자식에서 오버라이드 가능)
	CheckBuff = true;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: Buff On"));
}

void UDopingParent::BuffOff(UPlayerInformData* PID)
{
	// 기본 버프 종료 로직
	CheckBuff = false;

	// 디버프 시작
	DeBuffOn(PID);
	StartDeBuff(PID);

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: Buff Off"));
}

void UDopingParent::DeBuffOn(UPlayerInformData* PID)
{
	// 기본 디버프 로직
	CheckDeBuff = true;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: DeBuff On"));
}

void UDopingParent::DeBuffOff(UPlayerInformData* PID)
{
	// 기본 디버프 종료 로직
	CheckDeBuff = false;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: DeBuff Off"));
}

void UDopingParent::StartBuff(UPlayerInformData* PID)
{
	GetWorld()->GetTimerManager().SetTimer(
		BuffTimerHandle,
		[this, PID]() { EndBuff(PID); },
		BuffDuration,
		false
	);
}

void UDopingParent::StartDeBuff(UPlayerInformData* PID)
{
	GetWorld()->GetTimerManager().SetTimer(
		DeBuffTimerHandle,
		[this, PID]() { EndDeBuff(PID); },
		DeBuffDuration,
		false
	);
}

void UDopingParent::StartCooldown()
{
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		[this]() { EndCooldown(); },
		DopingCoolTime,
		false
	);
}

void UDopingParent::EndBuff(UPlayerInformData* PID)
{
	BuffOff(PID);
}

void UDopingParent::EndDeBuff(UPlayerInformData* PID)
{
	DeBuffOff(PID);
}

void UDopingParent::EndCooldown()
{
	Able = true;
}
