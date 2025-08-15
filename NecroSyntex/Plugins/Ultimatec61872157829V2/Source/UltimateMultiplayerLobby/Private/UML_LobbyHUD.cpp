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
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("onloggedin start"));
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("getsubsystem onlinesubsystem"));
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("getsessioninterface"));
			if(FOnlineSession* SessionInfo = SessionPtr->GetNamedSession(NAME_PartySession))
			{
				GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("getnamedsession"));
				if(LoginInProgressWidget)
				{
					GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("logininprogresswidget"));
					LoginInProgressWidget->RemoveFromParent();
				}	
				if(LobbyMenuWidgetClass)
				{
					GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("lobbymenuwidgetclass"));
					LobbyMenuWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyMenuWidgetClass);
					if(LobbyMenuWidget)
					{
						GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("lobbymenuwidget start"));
						LobbyMenuWidget->AddToViewport();
					}
					//GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("no lobbymenuwidget"));
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
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("onloginprocessstarted"));
	if(LoginInProgressWidgetClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("logininprogresswidgetclass"));
		LoginInProgressWidget = CreateWidget<UUserWidget>(GetWorld(), LoginInProgressWidgetClass);
		if(LoginInProgressWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("logininprogresswidget add to viewport"));
			LoginInProgressWidget->AddToViewport();
		}
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("OnLoginProcessStarted_Implementation1"));
		if(IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface())
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("OnLoginProcessStarted_Implementation2"));
			Identity->AutoLogin(0);
		}
	}
	return true;
}

bool AUML_LobbyHUD::OnManualLoginRequired_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("OnManualLoginRequired_Implementation"));
	if(LoginInProgressWidgetClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("OnManualLoginRequired_Implementation2"));
		LoginInProgressWidget = CreateWidget<UUserWidget>(GetWorld(), LoginInProgressWidgetClass);
		if(LoginInProgressWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("OnManualLoginRequired_Implementation3"));
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

	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("beginplay"));

	if(UUML_OnlineFunctions::IsLoggedIn(this))
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("if isloggedin"));
		OnLoggedIn();
	}
	else
	{
		if(bAutoLogin)
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("autologin"));
			OnLoginProcessStarted();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, TEXT("no autologin"));
			OnManualLoginRequired();
		}
		GetWorld()->GetTimerManager().SetTimer(LoginStatusCheckTimerHandle, this, &AUML_LobbyHUD::CheckLoginStatus, 2.0f, true);
	}
}
