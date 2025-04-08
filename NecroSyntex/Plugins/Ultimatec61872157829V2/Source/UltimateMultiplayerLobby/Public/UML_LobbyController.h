// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UML_CharacterConfig.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerStart.h"
#include "UML_LobbyController.generated.h"

USTRUCT(BlueprintType)
struct FUML_ChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Lobby Controller")
	FString Sender;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Lobby Controller")
	FString Message;
};

UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API AUML_LobbyController : public APlayerController
{
	GENERATED_BODY()

public:


	/********************* Session System *****************************/

	//Should the Session be advertised
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	bool bShouldAdvertise = true;

	//Should the Session use presence
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	bool bUsesPresence = true;

	//Number of public connections
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	int32 NumPublicConnections = 4;

	//Should invites be allowed
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	bool bAllowInvites = true;

	//Should the session be using voice
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	bool bUseLobbiesVoiceChatIfAvailable = false;

	//Allow join via presence
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller|Session")
	bool bAllowJoinViaPresence = true;

	// Send a text message to the lobby, this will handle the replication of the message to all clients, so which means you can call it on both the client and server
	UFUNCTION(BlueprintCallable, Category = "Ultimate Multiplayer Lobby|Lobby Controller")
	void SendTextMessage(const FString& Message);

	// Server function to send a text message to the lobby
	UFUNCTION(Server, Reliable)
	void ServerSendTextMessage(const FString& Message);

	/********************* Ready System *****************************/

	// Check if everyone in the lobby is ready, this will return true if everyone is ready, false if not
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Lobby Controller")
	bool IsEveryoneReadyInLobby();

	// Server function to set the ready state of the player
	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bReady);

	// Set the ready state of the player, this will handle the replication of the ready state to all clients, so which means you can call it on both the client and server
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Lobby Controller")
	void SetReady(bool bReady);

	// Check if the player is ready, this will be the local ready state of the player
	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Lobby Controller")
	bool IsReady() const;

	/********************* Host System *****************************/

	// This is a Native event which can be overridden in blueprints, this will be called when the player wants to host a lobby
	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby Controller")
	void HostLobby();

	// This is a Native event which can be overridden in blueprints, this will be called when the host lobby failed to host
	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby Controller")
	void OnHostLobbyFailed();

	// This is a Native event which can be overridden in blueprints, this will be called when the host lobby successfully hosted
	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby Controller")
	void OnHostLobbySuccess();

	// This function will be called when the player is ready to host the lobby
	void ReadyToHostLobby();

	/********************* Variables *****************************/

	// The character config for the lobby, this will be used to spawn the default character in the lobby and to get the available characters
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby Controller")
	UUML_CharacterConfig* LobbyCharacterConfig;

	/********************* Material Apply *****************************/

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Lobby Controller")
	void ChangeCharacter(FUML_AvailableCharacterConfig CharacterConfig);
	
	UFUNCTION(Server, Reliable)
	void ServerChangeCharacter(FUML_AvailableCharacterConfig CharacterConfig);

	UFUNCTION(Client, Reliable)
	void ClientChangeCharacter(FUML_AvailableCharacterConfig CharacterConfig);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Lobby Controller")
	void KickPlayerFromLobby(AUML_LobbyController* ControllerRef);

	UFUNCTION(Client, Reliable)
	void ClientKickPlayer();
	
private:	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void OnSessionUserInviteAccepted(bool bWasSuccesful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);
	FDelegateHandle OnKickedSessionDestroyHandle;
	FDelegateHandle OnInviteAcceptedDestroyHandle;
	FDelegateHandle OnInviteAcceptedJoinHandle;
	FDelegateHandle OnHostLobbyCreateHandle;
	UPROPERTY()
	APlayerStart* PlayerStartToBeUsed;
};
