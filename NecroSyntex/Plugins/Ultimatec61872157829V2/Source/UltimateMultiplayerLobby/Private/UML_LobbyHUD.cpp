// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "UML_LobbyHUD.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "UML_LobbyController.h"
#include "UML_OnlineFunctions.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

bool AUML_LobbyHUD::OnLoggedIn_Implementation()
{
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			if(FOnlineSession* SessionInfo = SessionPtr->GetNamedSession(NAME_PartySession))
			{
				if(LoginInProgressWidget)
				{
					LoginInProgressWidget->RemoveFromParent();
				}	
				if(LobbyMenuWidgetClass)
				{
					LobbyMenuWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyMenuWidgetClass);
					if(LobbyMenuWidget)
					{
						LobbyMenuWidget->AddToViewport();
					}
				}
			}
		}
	}
	if(GetOwningPlayerController())
	{
		if(AUML_LobbyController* LobbyController = Cast<AUML_LobbyController>(GetOwningPlayerController()))
		{
			LobbyController->ReadyToHostLobby();
		}
	}
	return true;
}

bool AUML_LobbyHUD::OnLoggedOut_Implementation()
{
	return false;
}

bool AUML_LobbyHUD::OnLoginFailed_Implementation()
{
	return false;
}

bool AUML_LobbyHUD::OnLoginProcessStarted_Implementation()
{
	if(LoginInProgressWidgetClass)
	{
		LoginInProgressWidget = CreateWidget<UUserWidget>(GetWorld(), LoginInProgressWidgetClass);
		if(LoginInProgressWidget)
		{
			LoginInProgressWidget->AddToViewport();
		}
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface())
		{
			Identity->AutoLogin(0);
		}
	}
	return true;
}

bool AUML_LobbyHUD::OnManualLoginRequired_Implementation()
{
	if(LoginInProgressWidgetClass)
	{
		LoginInProgressWidget = CreateWidget<UUserWidget>(GetWorld(), LoginInProgressWidgetClass);
		if(LoginInProgressWidget)
		{
			LoginInProgressWidget->AddToViewport();
			return true;
		}
	}
	return false;
}

void AUML_LobbyHUD::CheckLoginStatus()
{
	if(UUML_OnlineFunctions::IsLoggedIn(this))
	{
		GetWorld()->GetTimerManager().ClearTimer(LoginStatusCheckTimerHandle);
		OnLoggedIn();
	}
}

void AUML_LobbyHUD::BeginPlay()
{
	Super::BeginPlay();
	if(UUML_OnlineFunctions::IsLoggedIn(this))
	{
		OnLoggedIn();
	}
	else
	{
		if(bAutoLogin)
		{
			OnLoginProcessStarted();
		}
		else
		{
			OnManualLoginRequired();
		}
		GetWorld()->GetTimerManager().SetTimer(LoginStatusCheckTimerHandle, this, &AUML_LobbyHUD::CheckLoginStatus, 2.0f, true);
	}
}
