// Copyright (c) 2025 Betide Studio. All Rights Reserved.

#include "UML_Subsystem.h"

#include "FindSessionsCallbackProxy.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

bool UUML_Subsystem::FindMultiplayerSessions(bool bUseLobbies, bool bUsePresence)
{
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineSessionPtr SessionPtr = SubsystemRef->GetSessionInterface())
		{
			TSharedRef<FOnlineSessionSearch> LocalSearchSettings = MakeShared<FOnlineSessionSearch, ESPMode::ThreadSafe>();
			DelegateHandleForFindSession = SessionPtr->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateLambda(
			   [this, LocalSearchSettings, SessionPtr](bool bWasSuccessful)
			   {
			   		DelegateHandleForFindSession.Reset();
				   if (bWasSuccessful)
				   {
					   TArray<FBlueprintSessionResult> SessionResults;
					   for(auto Session : LocalSearchSettings->SearchResults)
					   {
						   FBlueprintSessionResult NewSession;
						   NewSession.OnlineResult = Session;
						   SessionResults.Add(NewSession);
					   }
					   OnFindSessionsComplete.Broadcast(true, SessionResults);
				   }
				   else
				   {
					   OnFindSessionsComplete.Broadcast(false, TArray<FBlueprintSessionResult>());
				   }
			   }));
			LocalSearchSettings->bIsLanQuery = false;
			LocalSearchSettings->MaxSearchResults = 100;
			if(bUseLobbies)
			{
				LocalSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, bUseLobbies, EOnlineComparisonOp::Equals);
			}
			if(bUsePresence)
			{
				LocalSearchSettings->QuerySettings.Set(SEARCH_PRESENCE, bUsePresence, EOnlineComparisonOp::Equals);
			}
			return SessionPtr->FindSessions(0, LocalSearchSettings);
		}
	}
	return false;
}

bool UUML_Subsystem::CreateSession(bool bUseLobbies, int32 NumberOfPrivateConnections, int32 NumberOfPublicConnections,
	bool bUsePresence, bool bAllowInvites, bool bUseVoiceChat)
{
	if(const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
	{
		if(const IOnlineSessionPtr SessionPtr = SubsystemRef->GetSessionInterface())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bUsesPresence = bUsePresence;
			SessionSettings.bAllowInvites = bAllowInvites;
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bIsLANMatch = false;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.NumPublicConnections = NumberOfPublicConnections;
			SessionSettings.NumPrivateConnections = NumberOfPrivateConnections;
			SessionSettings.bUseLobbiesVoiceChatIfAvailable = bUseVoiceChat;
			SessionSettings.bUseLobbiesIfAvailable = bUseLobbies;
			DelegateHandleForCreateSession = SessionPtr->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateLambda(
				[this, SessionPtr](FName SessionName, bool bWasSuccessful)
				{
					DelegateHandleForCreateSession.Reset();
					if(bWasSuccessful)
					{
						OnCreateSessionComplete.Broadcast(true, SessionName.ToString());
					}
					else
					{
						OnCreateSessionComplete.Broadcast(false, "");
					}
				}));
			return SessionPtr->CreateSession(0, NAME_GameSession, SessionSettings);
		}
	}
	return false;
}

bool UUML_Subsystem::DestroyMultiplayerSession(FString SessionName)
{
	if(const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
	{
		if(const IOnlineSessionPtr SessionPtr = SubsystemRef->GetSessionInterface())
		{
			DelegateHandleForDestroySession = SessionPtr->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateLambda(
				[this](FName SessionName, bool bWasSuccessful)
				{
					DelegateHandleForDestroySession.Reset();
					if(bWasSuccessful)
					{
						OnDestroySessionComplete.Broadcast(true);
					}
					else
					{
						OnDestroySessionComplete.Broadcast(false);
					}
				}));
			return SessionPtr->DestroySession(NAME_GameSession);
		}
	}
	return false;
}


bool UUML_Subsystem::StartBasicGameMatchmaking(TMap<FString, FString> ExtraSettings, bool bUseLobbies, bool bUsePresence,
    int32 NumberOfPublicConnections, int32 NumberOfPrivateConnections, bool bUseVoiceChat, bool bAllowInvites,
    bool bShouldAdvertise)
{
	this->VarNumberOfPublicConnections = NumberOfPublicConnections;
	this->VarNumberOfPrivateConnections = NumberOfPrivateConnections;
	this->bVarUseLobbies = bUseLobbies;
	this->bVarUsePresence = bUsePresence;
	this->bVarShouldAdvertise = bShouldAdvertise;
	this->bVarUseVoiceChat = bUseVoiceChat;
	this->bVarAllowInvites = bAllowInvites;
    CopiedExtraSettings = ExtraSettings;
    const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld());
    if (!SubsystemRef)
    {
        UE_LOG(LogTemp, Error, TEXT("Online subsystem not found."));
        OnStartMatchmakingComplete.Broadcast(false, false, FBlueprintSessionResult(), "");
        return false;
    }

    const IOnlineSessionPtr SessionPtr = SubsystemRef->GetSessionInterface();
    if (!SessionPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("Online session interface not found."));
        OnStartMatchmakingComplete.Broadcast(false, false, FBlueprintSessionResult(), "");
        return false;
    }

    bIsMatchmakingCanceled = false;
    bIsMatchmakingInProgress = true;

    SearchSettings = MakeShared<FOnlineSessionSearch, ESPMode::ThreadSafe>();
    SearchSettings->bIsLanQuery = false;
    SearchSettings->MaxSearchResults = 20;
    SearchSettings->TimeoutInSeconds = 20.f;
    if (bUseLobbies)
    {
        SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, bUseLobbies, EOnlineComparisonOp::Equals);
    }
    if (bUsePresence)
    {
        SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, bUsePresence, EOnlineComparisonOp::Equals);
    }
    for (const auto& KeyValuePair : ExtraSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("Adding search setting: Key=%s, Value=%s"), *KeyValuePair.Key, *KeyValuePair.Value);
        SearchSettings->QuerySettings.Set(FName(*KeyValuePair.Key), KeyValuePair.Value, EOnlineComparisonOp::Equals);
    }

    DelegateHandleForFindSession = SessionPtr->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UUML_Subsystem::HandleFindSessionsComplete));

    return SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
}

void UUML_Subsystem::HandleFindSessionsComplete(bool bWasSuccessful)
{
    const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld());
    const IOnlineSessionPtr SessionPtr = SubsystemRef ? SubsystemRef->GetSessionInterface() : nullptr;

    if (bIsMatchmakingCanceled || !SessionPtr)
    {
        if (SessionPtr)
        {
            SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandleForFindSession);
        }
        bIsMatchmakingInProgress = false;
        OnStartMatchmakingComplete.Broadcast(false, false, FBlueprintSessionResult(), "");
        return;
    }

    SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandleForFindSession);
    FOnlineSessionSearchResult DecidedSearchResult;

    if (bWasSuccessful && SearchSettings.IsValid() && !SearchSettings->SearchResults.IsEmpty())
    {
        for (const auto& LocalSession : SearchSettings->SearchResults)
        {
            if (LocalSession.IsValid() &&
                (LocalSession.Session.NumOpenPublicConnections + LocalSession.Session.NumOpenPrivateConnections) !=
                (LocalSession.Session.SessionSettings.NumPublicConnections + LocalSession.Session.SessionSettings.NumPrivateConnections))
            {
                DecidedSearchResult = LocalSession;
                UE_LOG(LogTemp, Warning, TEXT("Session found with open connections: %d"),
                       LocalSession.Session.NumOpenPublicConnections + LocalSession.Session.NumOpenPrivateConnections);
                break;
            }
        }
    }

    if (DecidedSearchResult.IsValid())
    {
        FBlueprintSessionResult SessionRef;
        SessionRef.OnlineResult = DecidedSearchResult;
        bIsMatchmakingInProgress = false;
        OnStartMatchmakingComplete.Broadcast(false, true, SessionRef, "GameSession");
        CurrentRetry = 0;
    }
    else
    {
        HandleMatchmakingRetry(SessionPtr);
    }
}

void UUML_Subsystem::HandleMatchmakingRetry(const IOnlineSessionPtr& SessionPtr)
{
    CurrentRetry++;
    if (CurrentRetry < MaxRetries)
    {
        UE_LOG(LogTemp, Warning, TEXT("No sessions found. Retrying (%d/%d)..."), CurrentRetry, MaxRetries);
        StartBasicGameMatchmaking(CopiedExtraSettings, bVarUseLobbies, bVarUsePresence, 
                                  VarNumberOfPublicConnections, VarNumberOfPrivateConnections, 
                                  bVarUseVoiceChat, bVarAllowInvites, bVarShouldAdvertise);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Max retries reached. Creating a new session..."));
        CreateNewSession(SessionPtr);
    }
}

void UUML_Subsystem::CreateNewSession(const IOnlineSessionPtr& SessionPtr)
{
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bUsesPresence = bVarUsePresence;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.bShouldAdvertise = bVarShouldAdvertise;
    SessionSettings.NumPublicConnections = VarNumberOfPublicConnections;
    SessionSettings.NumPrivateConnections = VarNumberOfPrivateConnections;
    SessionSettings.bUseLobbiesIfAvailable = bVarUseLobbies;

    for (const auto& KeyValuePair : CopiedExtraSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("Adding session setting: Key=%s, Value=%s"), *KeyValuePair.Key, *KeyValuePair.Value);
        SessionSettings.Set(FName(*KeyValuePair.Key), KeyValuePair.Value, EOnlineDataAdvertisementType::ViaOnlineService);
    }

    DelegateHandleForCreateSession = SessionPtr->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UUML_Subsystem::HandleCreateSessionComplete));

    SessionPtr->CreateSession(0, NAME_GameSession, SessionSettings);
}

void UUML_Subsystem::HandleCreateSessionComplete(FName CreatedSessionName, bool bWasSuccessful)
{
    const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld());
    const IOnlineSessionPtr SessionPtr = SubsystemRef ? SubsystemRef->GetSessionInterface() : nullptr;

    if (!SessionPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("SessionPtr is invalid during delegate execution."));
        return;
    }

    SessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandleForCreateSession);

    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session created successfully: %s"), *CreatedSessionName.ToString());
        OnStartMatchmakingComplete.Broadcast(true, false, FBlueprintSessionResult(), CreatedSessionName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session."));
        OnStartMatchmakingComplete.Broadcast(false, false, FBlueprintSessionResult(), "");
    }
    bIsMatchmakingInProgress = false;
}

void UUML_Subsystem::CancelMatchmaking()
{
    bIsMatchmakingCanceled = true;

    // Optionally clear delegates to ensure no further matchmaking logic runs
    if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
    {
	    if (const IOnlineSessionPtr SessionPtr = SubsystemRef->GetSessionInterface())
        {
            SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandleForFindSession);
            SessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandleForCreateSession);
        }
    }
    OnStartMatchmakingComplete.Broadcast(false, false, FBlueprintSessionResult(), "");
}
