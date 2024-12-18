/*
* �� �÷��̾�� ���� �ý���
*/

#include "NecroSyntexPlayerState.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"

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

