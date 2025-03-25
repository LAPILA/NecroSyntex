/*
* �� �÷��̾�� ���� �ý���
*/

#include "NecroSyntexPlayerState.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "NecroSyntex\PlayerController\NecroSyntexPlayerController.h"
#include "Net/UnrealNetwork.h"

ANecroSyntexPlayerState::ANecroSyntexPlayerState()
{
	bAlwaysRelevant = true;
	bReplicates = true;
}

void ANecroSyntexPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANecroSyntexPlayerState, Defeats);


	DOREPLIFETIME(ANecroSyntexPlayerState, SelectedCharacterClass);
	DOREPLIFETIME(ANecroSyntexPlayerState, FirstDopingCode);
	DOREPLIFETIME(ANecroSyntexPlayerState, SecondDopingCode);
	DOREPLIFETIME(ANecroSyntexPlayerState, bHasCompletedSelection);
	DOREPLIFETIME(ANecroSyntexPlayerState, CopyComplete);
}

void ANecroSyntexPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	//Check Player Controller
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
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
			Controller->SetHUDScore(Score);
		}
	}
}

void ANecroSyntexPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character)
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
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ANecroSyntexPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ANecroSyntexPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);


	UE_LOG(LogTemp, Warning, TEXT("CopyProperty 작동"));
	ANecroSyntexPlayerState* NewPS = Cast<ANecroSyntexPlayerState>(PlayerState);
	if (NewPS) {
		NewPS->SelectedCharacterClass = SelectedCharacterClass;
		NewPS->FirstDopingCode = FirstDopingCode;
		NewPS->SecondDopingCode = SecondDopingCode;
		NewPS->CopyComplete = true;
		CopyComplete = true;
	}
}