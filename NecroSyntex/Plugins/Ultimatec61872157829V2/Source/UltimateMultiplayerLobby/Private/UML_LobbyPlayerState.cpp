// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "UML_LobbyPlayerState.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void AUML_LobbyPlayerState::SetReady(bool bReady)
{
	if(HasAuthority())
	{
		bIsReady = bReady;
	}
	else
	{
		ServerSetReady(bReady);
	}
}

void AUML_LobbyPlayerState::ServerSetReady_Implementation(bool bReady)
{
	bIsReady = bReady;
}

void AUML_LobbyPlayerState::OnRep_bIsReady()
{
	UE_LOG(LogTemp, Warning, TEXT("Player %s is ready: %s"), *GetPlayerName(), bIsReady ? TEXT("true") : TEXT("false"));
}

void AUML_LobbyPlayerState::UseEmote(UAnimMontage* EmoteToPlay)
{
	if(!EmoteToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Emote To Play Set"));
		return;
	}
	
	if(HasAuthority())
	{
		MulticastUseEmote(EmoteToPlay);
	}
	else
	{
		ServerUseEmote(EmoteToPlay);
	}
}

void AUML_LobbyPlayerState::ServerUseEmote_Implementation(UAnimMontage* EmoteToPlay)
{
	MulticastUseEmote(EmoteToPlay);
}

void AUML_LobbyPlayerState::MulticastUseEmote_Implementation(UAnimMontage* EmoteToPlay)
{
	if(GetPawn())
	{
		if(ACharacter* Character = Cast<ACharacter>(GetPawn()))
		{
			Character->PlayAnimMontage(EmoteToPlay);
		}
	}
}

void AUML_LobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUML_LobbyPlayerState, bIsReady);
}
