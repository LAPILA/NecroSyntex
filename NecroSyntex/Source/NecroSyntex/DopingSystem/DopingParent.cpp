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

void UDopingParent::UseDopingItem(APlayerCharacter* DopedPC)
{
	// Buff 시작
	BuffOn(DopedPC);
	DeBuffOn(DopedPC);
}

void UDopingParent::BuffOn(APlayerCharacter* DopedPC)
{
	// 기본 버프 로직 (자식에서 오버라이드 가능)
	CheckBuff = true;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: Buff On"));
}

void UDopingParent::BuffOff(APlayerCharacter* DopedPC)
{
	// 기본 버프 종료 로직
	CheckBuff = false;

	// 디버프 시작
	DeBuffOn(DopedPC);
	StartDeBuff(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: Buff Off"));
}

void UDopingParent::DeBuffOn(APlayerCharacter* DopedPC)
{
	// 기본 디버프 로직
	CheckDeBuff = true;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: DeBuff On"));
}

void UDopingParent::DeBuffOff(APlayerCharacter* DopedPC)
{
	// 기본 디버프 종료 로직
	CheckDeBuff = false;

	UE_LOG(LogTemp, Warning, TEXT("DopingParent: DeBuff Off"));
}

void UDopingParent::StartBuff(APlayerCharacter* DopedPC)
{
	GetWorld()->GetTimerManager().SetTimer(
		BuffTimerHandle,
		[this, DopedPC]() { EndBuff(DopedPC); },
		BuffDuration,
		false
	);
}

void UDopingParent::StartDeBuff(APlayerCharacter* DopedPC)
{
	GetWorld()->GetTimerManager().SetTimer(
		DeBuffTimerHandle,
		[this, DopedPC]() { EndDeBuff(DopedPC); },
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

void UDopingParent::EndBuff(APlayerCharacter* DopedPC)
{
	BuffOff(DopedPC);
}

void UDopingParent::EndDeBuff(APlayerCharacter* DopedPC)
{
	DeBuffOff(DopedPC);
}

void UDopingParent::EndCooldown()
{
	Able = true;
}

void UDopingParent::TimerEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(BuffTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DeBuffTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}
