// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "TimerManager.h"
#include "UML_LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API AUML_LobbyHUD : public AHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ultimate Game Menu|Lobby HUD")
	bool OnLoggedIn();

	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby HUD")
	bool OnLoggedOut();

	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby HUD")
	bool OnLoginFailed();

	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby HUD")
	bool OnLoginProcessStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "Ultimate Game Menu|Lobby HUD")
	bool OnManualLoginRequired();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby HUD")
	TSubclassOf<UUserWidget> LoginInProgressWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ultimate Game Menu|Lobby HUD")
	TSubclassOf<UUserWidget> LobbyMenuWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Ultimate Game Menu|Lobby HUD")
	UUserWidget* LoginInProgressWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Ultimate Game Menu|Lobby HUD")
	UUserWidget* LobbyMenuWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ultimate Game Menu|Lobby HUD")
	bool bAutoLogin = true;
	

protected:
	FTimerHandle LoginStatusCheckTimerHandle;
	void CheckLoginStatus();
	virtual void BeginPlay() override;
};
