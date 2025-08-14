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

	CheckBuff = false;
	CheckDeBuff = false;

}

bool UDPLegEnforce::UseDopingItem(APlayerCharacter* DopedPC)
{
	// Buff 적용 및 타이머 시작
	BuffOn(DopedPC);

	return true;
}

void UDPLegEnforce::BuffOn(APlayerCharacter* DopedPC)
{
	if (CheckBuff == false) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("use buff"));
		
		DopedPC->WalkSpeed += WalkingBuffNum;
		DopedPC->RunningSpeed += RunningBuffNum;

		CheckBuff = true;

		FString BuffValue = CheckBuff ? TEXT("true") : TEXT("false");
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("use buff, CheckBuff = %s"), *BuffValue));

		DopedPC->CurrentDoped += 1;

	}

	StartBuff(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce BuffOn: Speed increased."));
}

void UDPLegEnforce::BuffOff(APlayerCharacter* DopedPC)
{
	if (CheckBuff == true) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("off buff"));
		DopedPC->WalkSpeed -= WalkingBuffNum;
		DopedPC->RunningSpeed -= RunningBuffNum;

		CheckBuff = false;

		// DeBuff 적용 및 타이머 시작
		DeBuffOn(DopedPC);
	}

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce BuffOff: Speed normalized."));
}

void UDPLegEnforce::DeBuffOn(APlayerCharacter* DopedPC)
{

	if (CheckDeBuff == false) {
		DopedPC->WalkSpeed -= WalkingDeBuffNum;
		DopedPC->RunningSpeed -= RunningDeBuffNum;

		CheckDeBuff = true;
	}

	StartDeBuff(DopedPC);

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce DeBuffOn: Speed reduced."));
}

void UDPLegEnforce::DeBuffOff(APlayerCharacter* DopedPC)
{
	if (CheckDeBuff == true) {
		DopedPC->WalkSpeed += WalkingDeBuffNum;
		DopedPC->RunningSpeed += RunningDeBuffNum;

		DopedPC->CurrentDoped -= 1;

		CheckDeBuff = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("LegEnforce DeBuffOff: Speed restored."));
}
