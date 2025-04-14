// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UML_LobbyController.h"
#include "GameFramework/GameStateBase.h"
#include "UML_LobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUML_OnChatMessageReceived, FUML_ChatMessage, ChatMessage);
UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API AUML_LobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Lobby Controller")
	FUML_OnChatMessageReceived OnChatMessageReceived;
	
	UFUNCTION(Server, Reliable)
	void DistributeNewChatMessage(FUML_ChatMessage ChatMessage);

	UFUNCTION(NetMulticast, Reliable)
	void Mlt_DistributeNewChatMessage(FUML_ChatMessage ChatMessage);
};
