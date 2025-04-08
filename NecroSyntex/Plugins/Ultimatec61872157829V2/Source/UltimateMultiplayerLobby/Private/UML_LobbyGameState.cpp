// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "UML_LobbyGameState.h"

void AUML_LobbyGameState::DistributeNewChatMessage_Implementation(FUML_ChatMessage ChatMessage)
{
	Mlt_DistributeNewChatMessage(ChatMessage);
}

void AUML_LobbyGameState::Mlt_DistributeNewChatMessage_Implementation(FUML_ChatMessage ChatMessage)
{
	OnChatMessageReceived.Broadcast(ChatMessage);
}
