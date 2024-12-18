/*
* 각 플레이어당 점수 시스템
*/

#include "NecroSyntexPlayerState.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Net/UnrealNetwork.h"

void ANecroSyntexPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANecroSyntexPlayerState, Defeats);
}

void ANecroSyntexPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;

	//Check Player Controller
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHudScore(Score);
		}
	}
}

void ANecroSyntexPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	//Check Player Controller
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHudScore(Score);
		}
	}
}

void ANecroSyntexPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
void ANecroSyntexPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}