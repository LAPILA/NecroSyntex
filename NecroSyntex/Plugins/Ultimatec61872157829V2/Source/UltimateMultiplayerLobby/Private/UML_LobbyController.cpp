// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "UML_LobbyController.h"

#include "CineCameraActor.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "UML_LobbyGameState.h"
#include "OnlineSubsystemUtils.h"
#include "UML_LobbyPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Runtime/Core/Public/Async/Async.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

void AUML_LobbyController::SendTextMessage(const FString& Message)
{
	ServerSendTextMessage(Message);
}

void AUML_LobbyController::ServerSendTextMessage_Implementation(const FString& Message)
{
	FString SenderName = PlayerState->GetPlayerName();
	FUML_ChatMessage ChatMessage;
	ChatMessage.Sender = SenderName;
	ChatMessage.Message = Message;
	if(AUML_LobbyGameState* BaseGameState = GetWorld()->GetGameState<AUML_LobbyGameState>())
	{
		BaseGameState->DistributeNewChatMessage(ChatMessage);
	}
}

bool AUML_LobbyController::IsEveryoneReadyInLobby()
{
	if(HasAuthority())
	{
		if(AGameStateBase* GameState = GetWorld()->GetGameState())
		{
			if(GameState->PlayerArray.Num() > 1)
			{
				for(auto PlayerRef : GameState->PlayerArray)
				{
					if(AUML_LobbyPlayerState* BasePlayerState = Cast<AUML_LobbyPlayerState>(PlayerRef))
					{
						//Check if unique net id is same as host
						if(GameState->PlayerArray.Num() > 0)
						{
							if(BasePlayerState->GetUniqueId() == GameState->PlayerArray[0]->GetUniqueId())
							{
								continue;
							}
						}
						if(!BasePlayerState->bIsReady)
						{
							return false;
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

void AUML_LobbyController::ServerSetReady_Implementation(bool bReady)
{
	SetReady(bReady);
}

void AUML_LobbyController::SetReady(bool bReady)
{
	if(HasAuthority())
	{
		if(AUML_LobbyPlayerState* BasePlayerState = Cast<AUML_LobbyPlayerState>(PlayerState))
		{
			BasePlayerState->SetReady(bReady);
		}
	}
	else
	{
		ServerSetReady(bReady);
	}
}

bool AUML_LobbyController::IsReady() const
{
	if(AUML_LobbyPlayerState* BasePlayerState = Cast<AUML_LobbyPlayerState>(PlayerState))
	{
		return BasePlayerState->bIsReady;
	}
	return false;
}

void AUML_LobbyController::HostLobby_Implementation()
{
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			TWeakObjectPtr<AUML_LobbyController> WeakThis(this);
			OnHostLobbyCreateHandle = SessionPtr->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateLambda([WeakThis, SessionPtr, this](FName SessionName, bool bWasSuccessful)
			{
				SessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(OnHostLobbyCreateHandle);
				if (!WeakThis.IsValid())
				{
					return;
				}

				AUML_LobbyController* Controller = WeakThis.Get();
				if (bWasSuccessful)
				{
					if (!IsInGameThread())
					{
						AsyncTask(ENamedThreads::GameThread, [Controller, SessionName, bWasSuccessful]()
						{
							Controller->OnHostLobbySuccess();
						});
						return;
					}
					Controller->OnHostLobbySuccess();
				}
				else
				{
					Controller->OnHostLobbyFailed();
				}
			}));
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = false;
			SessionSettings.bShouldAdvertise = bShouldAdvertise;
			SessionSettings.bUsesPresence = bUsesPresence;
			SessionSettings.NumPublicConnections = NumPublicConnections;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bAllowInvites = bAllowInvites;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;	
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = bAllowJoinViaPresence;
			SessionPtr->CreateSession(0, NAME_PartySession, SessionSettings);
		}
	}
}

void AUML_LobbyController::OnHostLobbyFailed_Implementation()
{
}

void AUML_LobbyController::OnHostLobbySuccess_Implementation()
{
	if (!IsValid(this))
	{
		return;
	}
	UWorld* World = GetWorld();
	FString MapName = World ? World->GetMapName() : TEXT("Unknown");
	if (MapName.IsEmpty())
	{
		return;
	}
	ConsoleCommand(FString::Printf(TEXT("open %s?listen"), *MapName));
}


void AUML_LobbyController::ReadyToHostLobby()
{
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			if(FOnlineSession* SessionInfo = SessionPtr->GetNamedSession(NAME_PartySession))
			{
				SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &AUML_LobbyController::OnSessionUserInviteAccepted);
			}
			else
			{
				if(HasAuthority())
				{
					HostLobby();
				}
				else
				{
					SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &AUML_LobbyController::OnSessionUserInviteAccepted);
				}
			}
		}
	}
}

void AUML_LobbyController::ChangeCharacter(FUML_AvailableCharacterConfig CharacterConfig)
{
	if(HasAuthority())
	{
		FVector Location = GetPawn()->GetActorLocation();
		FRotator Rotation = GetPawn()->GetActorRotation();
		if(GetPawn())
		{
			GetPawn()->Destroy();
		}
		if(CharacterConfig.CharacterClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnParams.Owner = this;
			auto ItemRef = GetWorld()->SpawnActor<ACharacter>(CharacterConfig.CharacterClass, Location, Rotation, SpawnParams);
			Possess(ItemRef);
			ClientChangeCharacter(CharacterConfig);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character Not Found In Lobby Character Config"));
		}
	}
	else
	{
		ServerChangeCharacter(CharacterConfig);
	}
}	

void AUML_LobbyController::ServerChangeCharacter_Implementation(FUML_AvailableCharacterConfig CharacterConfig)
{
	ChangeCharacter(CharacterConfig);
}


void AUML_LobbyController::ClientChangeCharacter_Implementation(FUML_AvailableCharacterConfig CharacterConfig)
{
	if(IsLocalController())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACineCameraActor::StaticClass(), TEXT("LobbyCamera"), FoundActors);
		if(FoundActors.Num() > 0)
		{
			ACineCameraActor* CameraActor = Cast<ACineCameraActor>(FoundActors[0]);
			SetViewTarget(CameraActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Camera Actor Found. Please add a CineCameraActor to the level with the tag 'LobbyCamera'"));
		}
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void AUML_LobbyController::KickPlayerFromLobby(AUML_LobbyController* ControllerRef)
{
	if(HasAuthority())
	{
		if(ControllerRef)
		{
			ControllerRef->ClientKickPlayer();
		}
	}
}

void AUML_LobbyController::ClientKickPlayer_Implementation()
{
	// Ensure the controller is valid
	if (!IsValid(this))
	{
		UE_LOG(LogTemp, Error, TEXT("ClientKickPlayer: Controller is invalid."));
		return;
	}

	// Ensure the world context is valid
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("ClientKickPlayer: GetWorld() returned null."));
		return;
	}

	if (IsLocalController())
	{
		if (IOnlineSubsystem* OnlineSub = Online::GetSubsystem(World))
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
			{
				OnKickedSessionDestroyHandle = SessionPtr->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateLambda([this, SessionPtr](FName SessionName, bool bWasSuccessful)
				{
					UE_LOG(LogTemp, Warning, TEXT("AddOnDestroy Triggereddddddd"));
					SessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(OnKickedSessionDestroyHandle);
					if (UWorld* InnerWorld = GetWorld()) 
					{
						UE_LOG(LogTemp, Log, TEXT("ClientKickPlayer: Opening LobbyMap."));
						UGameplayStatics::OpenLevel(InnerWorld, "LobbyMap");
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("ClientKickPlayer: Inner GetWorld() returned null."));
					}
				}));

				if (!SessionPtr->DestroySession(NAME_PartySession))
				{
					UE_LOG(LogTemp, Warning, TEXT("ClientKickPlayer: DestroySession failed."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ClientKickPlayer: Session interface is invalid."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ClientKickPlayer: Online subsystem is null."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ClientKickPlayer: Not the local controller."));
	}
}


void AUML_LobbyController::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		if(!LobbyCharacterConfig)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Lobby Character Config Set"));
			return;
		}
		
		if(LobbyCharacterConfig->AvailableCharacters.Num() > 0)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), TEXT("LobbyStart"), FoundActors);
			if(FoundActors.Num() > 0)
			{
				APlayerStart* PlayerStart = Cast<APlayerStart>(FoundActors[0]);
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				SpawnParams.Owner = this;
				if (LobbyCharacterConfig->AvailableCharacters.Num() > 0 &&	LobbyCharacterConfig->AvailableCharacters[0].CharacterClass)
				{
					auto ItemRef = GetWorld()->SpawnActor<ACharacter>(LobbyCharacterConfig->AvailableCharacters[0].CharacterClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParams);
					Possess(ItemRef);
					PlayerStartToBeUsed = PlayerStart;
					PlayerStart->Tags.Remove("LobbyStart");
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Character Not Found In Lobby Character Config"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No Player Start Found. Please add a PlayerStart to the level with the tag 'LobbyStart'"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Characters Found. Please add characters to the LobbyCharacterConfig"));
		}
	}
	if(IsLocalController())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACineCameraActor::StaticClass(), TEXT("LobbyCamera"), FoundActors);
		if(FoundActors.Num() > 0)
		{
			ACineCameraActor* CameraActor = Cast<ACineCameraActor>(FoundActors[0]);
			SetViewTarget(CameraActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Camera Actor Found. Please add a CineCameraActor to the level with the tag 'LobbyCamera'"));
		}
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void AUML_LobbyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACineCameraActor::StaticClass(), TEXT("LobbyCamera"), FoundActors);
	if(FoundActors.Num() > 0)
	{
		ACineCameraActor* CameraActor = Cast<ACineCameraActor>(FoundActors[0]);
		SetViewTarget(CameraActor);  
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Camera Actor Found. Please add a CineCameraActor to the level with the tag 'LobbyCamera'"));
	}
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
}

void AUML_LobbyController::OnUnPossess()
{
	if(HasAuthority() && PlayerStartToBeUsed)
	{
		PlayerStartToBeUsed->Tags.Add("LobbyStart");
	}
	Super::OnUnPossess();
}

void AUML_LobbyController::OnSessionUserInviteAccepted(bool bWasSuccesful, int32 ControllerId,
	TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if(!bWasSuccesful || !IsValid(this))
	{
		return;
	}

	UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("OnSessionUserInviteAccepted: GetWorld() returned null."));
		return;
	}

	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(World);
	if(!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("OnSessionUserInviteAccepted: Online subsystem is null."));
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface();
	if(!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("OnSessionUserInviteAccepted: Session interface is invalid."));
		return;
	}

	// Store invite result for use in lambdas
	FOnlineSessionSearchResult StoredInviteResult = InviteResult;

	// Create a weak pointer to ourselves to safely use in lambdas
	TWeakObjectPtr<AUML_LobbyController> WeakThis(this);

	OnInviteAcceptedDestroyHandle = SessionPtr->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateLambda([WeakThis, SessionPtr, StoredInviteResult](FName SessionName, bool bWasSuccessful)
		{
			// Clear the delegate handle first
			if(WeakThis.IsValid())
			{
				SessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(WeakThis->OnInviteAcceptedDestroyHandle);
			}
			
			// Safety check that we're still valid
			if(!WeakThis.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete: Controller is no longer valid"));
				return;
			}
			
			AUML_LobbyController* Controller = WeakThis.Get();
			
			Controller->OnInviteAcceptedJoinHandle = SessionPtr->AddOnJoinSessionCompleteDelegate_Handle(
				FOnJoinSessionCompleteDelegate::CreateLambda([WeakThis, SessionPtr](FName SessionName, EOnJoinSessionCompleteResult::Type Result)
				{
					// Clear the delegate handle first
					if(WeakThis.IsValid())
					{
						SessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(WeakThis->OnInviteAcceptedJoinHandle);
					}
					
					// Safety check that we're still valid
					if(!WeakThis.IsValid())
					{
						UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Controller is no longer valid"));
						return;
					}
					
					AUML_LobbyController* Controller = WeakThis.Get();
					UWorld* InnerWorld = Controller->GetWorld();
					
					if(!InnerWorld)
					{
						UE_LOG(LogTemp, Error, TEXT("OnJoinSessionComplete: GetWorld() returned null."));
						return;
					}
					
					if(Result == EOnJoinSessionCompleteResult::Success)
					{
						FString ConnectString;
						if(SessionPtr->GetResolvedConnectString(SessionName, ConnectString))
						{
							UGameplayStatics::OpenLevel(InnerWorld, FName(*ConnectString), true);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("OnJoinSessionComplete: GetResolvedConnectString failed."));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Join failed with result %d."), Result);
					}
				}));
			
			// Now that we've set up the join callback, initiate the join
			if(!SessionPtr->JoinSession(0, NAME_PartySession, StoredInviteResult))
			{
				UE_LOG(LogTemp, Error, TEXT("OnDestroySessionComplete: JoinSession call failed."));
				SessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(Controller->OnInviteAcceptedJoinHandle);
			}
		}));

	// Initiate the session destroy
	if(!SessionPtr->DestroySession(NAME_PartySession))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnSessionUserInviteAccepted: DestroySession failed."));
		SessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(OnInviteAcceptedDestroyHandle);
	}
}
