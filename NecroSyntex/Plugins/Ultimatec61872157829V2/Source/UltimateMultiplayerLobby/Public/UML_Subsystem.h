// Copyright (c) 2025 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UML_Subsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUML_OnFindSessionsComplete, bool, bWasSuccess, const TArray<FBlueprintSessionResult>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUML_OnCreateSessionComplete, bool, bWasSuccess, const FString&, SessionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUML_OnJoinSessionComplete, bool, bWasSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUML_OnDestroySessionComplete, bool, bWasSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUML_OnStartMatchmakingComplete, bool, bSessionCreated, bool, bSessionFound, FBlueprintSessionResult, SessionRef, FString, SessionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUML_OnMatchmakingCanceled, bool, bWasCanceled);
UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API UUML_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Online Functions")
	FUML_OnFindSessionsComplete OnFindSessionsComplete;

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Online Functions")
	FUML_OnCreateSessionComplete OnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Online Functions")
	FUML_OnJoinSessionComplete OnJoinSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Online Functions")
	FUML_OnDestroySessionComplete OnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Ultimate Game Menu|Online Functions")
	FUML_OnStartMatchmakingComplete OnStartMatchmakingComplete;
	
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers")
	bool FindMultiplayerSessions(bool bUseLobbies, bool bUsePresence);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers")
	bool CreateSession(bool bUseLobbies, int32 NumberOfPrivateConnections, int32 NumberOfPublicConnections, bool bUsePresence, bool bAllowInvites, bool bUseVoiceChat);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers")
	bool DestroyMultiplayerSession(FString SessionName = "GameSession");

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (AutoCreateRefTerm = "Settings"))
	bool StartBasicGameMatchmaking(TMap<FString, FString> ExtraSettings, bool bUseLobbies, bool bUsePresence, int32 NumberOfPublicConnections = 4, int32 NumberOfPrivateConnections = 0, bool bUseVoiceChat = false, bool bAllowInvites = true, bool bShouldAdvertise = true);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (AutoCreateRefTerm = "Settings"))
	void CancelMatchmaking();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Helpers")
	int32 MaxRetries = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Helpers")
	int32 CurrentRetry = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Helpers")
	bool bIsMatchmakingCanceled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Helpers")
	bool bIsMatchmakingInProgress = false;
private:
	FDelegateHandle DelegateHandleForFindSession;
	FDelegateHandle DelegateHandleForCreateSession;
	FDelegateHandle DelegateHandleForJoinSession;
	FDelegateHandle DelegateHandleForDestroySession;
	TMap<FString, FString> CopiedExtraSettings;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

	// Handlers for session operations
	void HandleFindSessionsComplete(bool bWasSuccessful);
	void HandleMatchmakingRetry(const IOnlineSessionPtr& SessionPtr);
	void CreateNewSession(const IOnlineSessionPtr& SessionPtr);
	void HandleCreateSessionComplete(FName CreatedSessionName, bool bWasSuccessful);

	// Settings for the session
	bool bVarUseLobbies;
	bool bVarUsePresence;
	bool bVarShouldAdvertise;
	int32 VarNumberOfPublicConnections;
	int32 VarNumberOfPrivateConnections;
	bool bVarUseVoiceChat;
	bool bVarAllowInvites;

	void Func_OnSessionUserInviteAccepted(bool bArg, int I, TSharedPtr<const FUniqueNetId> UniqueNetId, const FOnlineSessionSearchResult& OnlineSessionSearchResult);

};
