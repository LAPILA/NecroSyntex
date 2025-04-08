// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UML_LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API AUML_LobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Player State")
	void SetReady(bool bReady);

	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bReady);
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_bIsReady, Category = "Ultimate Game Menu|Player State")
	bool bIsReady = false;

	UFUNCTION()
	void OnRep_bIsReady();

	// This is a Native event which can be overridden in blueprints, this will be called when the player wants to use an emote
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Player State")
	void UseEmote(UAnimMontage* EmoteToPlay);

	UFUNCTION(Server, Reliable)
	void ServerUseEmote(UAnimMontage* EmoteToPlay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUseEmote(UAnimMontage* EmoteToPlay);
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
