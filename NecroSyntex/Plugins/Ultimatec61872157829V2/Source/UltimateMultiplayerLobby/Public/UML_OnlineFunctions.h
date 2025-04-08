// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "OnlineStats.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemUtils.h"
#include "UML_LobbyGameState.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UML_OnlineFunctions.generated.h"

USTRUCT(BlueprintType)
struct FUML_Friend
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Ultimate Game Menu|Online Functions")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category="Ultimate Game Menu|Online Functions")
	FString RealName;

	UPROPERTY(BlueprintReadOnly, Category="Ultimate Game Menu|Online Functions")
	FString Presence;

	UPROPERTY(BlueprintReadOnly, Category="Ultimate Game Menu|Online Functions")
	FUniqueNetIdRepl UniqueNetId;

	UPROPERTY(BlueprintReadOnly, Category="Ultimate Game Menu|Online Functions")
	bool bIsOnline = false;
};

USTRUCT(BlueprintType)
struct FUML_PlayerStat
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FString StatName;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FString StatValue;
};


USTRUCT(BlueprintType)
struct FUML_LeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	int32 Rank = 0;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	int32 Score = 0;
};

USTRUCT(BlueprintType)
struct FUML_Achievement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FString Id = "";

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	double Progress = 0.0;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FText Title = FText::FromString("");

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FText LockedDesc = FText::FromString("");
 
	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FText UnlockedDesc = FText::FromString("");

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	bool bIsHidden = false;

	UPROPERTY(BlueprintReadWrite, Category="Ultimate Game Menu|Online Functions")
	FDateTime UnlockTime;
	
};

USTRUCT(BlueprintType)
struct FUML_OffersStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FString ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FText LongDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FText RegularPriceText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	int64 RegularPrice = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FText PriceText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	int64 NumericPrice = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FDateTime ReleaseDate = FDateTime::MinValue();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FDateTime ExpirationDate = FDateTime::MinValue();

	// Image Ref for the offer, if you have image url, you can use it to download the image and set it below
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	UTexture2D* Image;

	// Image Url for the offer, if you have image url, you can use it to download the image and set it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate Game Menu|Online Functions")
	FString ImageUrl;
};



DECLARE_DYNAMIC_DELEGATE_OneParam(FUML_OnLeaderboardRefreshed, const TArray<FUML_LeaderboardEntry>&, LeaderboardEntries);
DECLARE_DYNAMIC_DELEGATE_OneParam(FUML_OnFriendsListRefreshed, const TArray<FUML_Friend>&, FriendsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FUML_OnPlayerStatsRefreshed, const TArray<FUML_PlayerStat>&, PlayerStats);
DECLARE_DYNAMIC_DELEGATE_OneParam(FUML_OnAchievementsRefreshed, const TArray<FUML_Achievement>&, Achievements);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FUML_OnOffersRefreshed, bool, bWasSuccess, const TArray<FUML_OffersStruct>&, Offers);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FUML_OnStorePurchaseComplete, bool, bWasSuccess, const FString&, TransactionId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FUM_OnInventoryRefreshed, bool, bWasSuccess, const TArray<FString>&, OwnedItems);
UCLASS()
class ULTIMATEMULTIPLAYERLOBBY_API UUML_OnlineFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static bool IsLoggedIn(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static FString GetPlayerNickname(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static void RefreshFriendsList(UObject* WorldContextObject, FUML_OnFriendsListRefreshed OnFriendsListRefreshed);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static void GetPlayerStats(UObject* WorldContextObject, const TArray<FString> StatName, FUML_OnPlayerStatsRefreshed OnPlayerStatsRefreshed);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static void SetPlayerStats(UObject* WorldContextObject, const FString& StatName, int32 StatValue);
	
	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static void InviteUserToGame(UObject* WorldContextObject, const FUML_Friend& Friend);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static bool IsInOnlineGameSession(UObject* WorldContextObject, FName SessionName = "PartySession");

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static bool IsFriendInOnlineGameSession(UObject* WorldContextObject, const FUniqueNetIdRepl& UniqueNetId);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static FUniqueNetIdRepl GetSessionOwner(UObject* WorldContextObject, FName SessionName, bool& bIsValid);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Online Functions", meta = (WorldContext = "WorldContextObject"))
	static bool IsUniqueNetIdEqual(const FUniqueNetIdRepl& UniqueNetId1, const FUniqueNetIdRepl& UniqueNetId2);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static AUML_LobbyGameState* GetBaseGameState(UObject* WorldContextObject)
	{
		if(!WorldContextObject)
		{
			return nullptr;
		}
		if(!WorldContextObject->GetWorld())
		{
			return nullptr;
		}
		if(!WorldContextObject->GetWorld()->GetGameState())
		{
			return nullptr;
		}
		if(AGameStateBase* GameState = WorldContextObject->GetWorld()->GetGameState())
		{
			return Cast<AUML_LobbyGameState>(GameState);
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static bool RequestPlayerProfileImage(UObject* WorldContextObject, const FUniqueNetIdRepl& UniqueNetId, UTexture2D* DefaultTexture, UTexture2D*& OutTexture, bool& RequestInProgress);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void GetLeaderboard(UObject* WorldContextObject, FName LeaderboardName, int32 Rank, int32 Range, FUML_OnLeaderboardRefreshed OnLeaderboardRefreshed);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void GetAchievements(UObject* WorldContextObject, FUML_OnAchievementsRefreshed OnAchievementsRefreshed);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void QueryOffers(UObject* WorldContextObject, FUML_OnOffersRefreshed OnOffersRefreshed);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void PurchaseOffer(UObject* WorldContextObject, const FString& OfferId, const FUML_OnStorePurchaseComplete& OnPurchaseComplete);

	UFUNCTION(BlueprintCallable, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void GetOwnedItems(UObject* WorldContextObject, const FUM_OnInventoryRefreshed& OnOwnedItemsRefreshed);

	UFUNCTION(BlueprintPure, Category = "Ultimate Game Menu|Helpers", meta = (WorldContext = "WorldContextObject"))
	static bool IsRunningSteamSubsystem(UObject* WorldContextObject);
	
};
