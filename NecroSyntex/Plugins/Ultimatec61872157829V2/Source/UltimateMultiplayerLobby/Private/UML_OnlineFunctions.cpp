// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "UML_OnlineFunctions.h"
#include "Runtime/Launch/Resources/Version.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "Online.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlinePurchaseInterface.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Engine/Texture2D.h"
#if USING_STEAM_INTEGRATION_KIT
#include <steam_api.h>
#elif USING_ONLINESUBSYSTEMSTEAM
#include "steam/steam_api.h"
#endif
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

bool UUML_OnlineFunctions::IsLoggedIn(UObject* WorldContextObject)
{
	if(!WorldContextObject)
	{
		return false;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface())
		{
			return Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn;
		}
	}
	return false;
}

FString UUML_OnlineFunctions::GetPlayerNickname(UObject* WorldContextObject)
{
	if(!WorldContextObject)
	{
		return FString();
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface())
		{
			return Identity->GetPlayerNickname(0);
		}
	}
	return FString();
}

void UUML_OnlineFunctions::RefreshFriendsList(UObject* WorldContextObject, FUML_OnFriendsListRefreshed OnFriendsListRefreshed)
{	
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineFriendsPtr Friends = OnlineSub->GetFriendsInterface())
		{
			FOnReadFriendsListComplete OnReadFriendsListComplete;
			OnReadFriendsListComplete.BindLambda([Friends, OnFriendsListRefreshed](int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
			{
				if(bWasSuccessful)
				{
					UE_LOG(LogTemp, Warning, TEXT("Friends list read successfully"));
					TArray< TSharedRef<FOnlineFriend> > FriendList;
					Friends->GetFriendsList(LocalUserNum, ListName, FriendList);
					if(FriendList.Num() <= 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("No friends found"));
						OnFriendsListRefreshed.ExecuteIfBound(TArray<FUML_Friend>());
						return;
					}
					TArray<FUML_Friend> FriendsList;
					for(auto Friend : FriendList)
					{
						FUML_Friend NewFriend;
						NewFriend.DisplayName = Friend->GetDisplayName();
						NewFriend.RealName = Friend->GetRealName();
						NewFriend.Presence = Friend->GetPresence().Status.StatusStr;
						NewFriend.bIsOnline = Friend->GetPresence().bIsOnline;
						NewFriend.UniqueNetId = Friend->GetUserId();
						FriendsList.Add(NewFriend);
					}
					OnFriendsListRefreshed.ExecuteIfBound(FriendsList);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to read friends list: %s"), *ErrorStr);
				}
			});
			Friends->ReadFriendsList(0, TEXT(""), OnReadFriendsListComplete);
		}
	}
}

void UUML_OnlineFunctions::GetPlayerStats(UObject* WorldContextObject, const TArray<FString> StatName, FUML_OnPlayerStatsRefreshed
                                          OnPlayerStatsRefreshed)
{
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(const IOnlineIdentityPtr IdentityPointerRef = OnlineSub->GetIdentityInterface())
		{
			if(const IOnlineStatsPtr StatsPointerRef = OnlineSub->GetStatsInterface())
			{
				
				TSharedPtr<const FUniqueNetId> UniqueNetId = IdentityPointerRef->GetUniquePlayerId(0);
				if (!UniqueNetId.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("UniqueNetId is invalid."));
					return;
				}
				TArray<TSharedRef<const FUniqueNetId>> Usersvar;
				Usersvar.Add(UniqueNetId.ToSharedRef());
				StatsPointerRef->QueryStats(IdentityPointerRef->GetUniquePlayerId(0).ToSharedRef(),Usersvar,StatName,FOnlineStatsQueryUsersStatsComplete::CreateLambda([OnPlayerStatsRefreshed](const FOnlineError& ResultState,
					const TArray<TSharedRef<const FOnlineStatsUserStats>>& UsersStatsResult)
				{
					if(ResultState.WasSuccessful())
					{
						TArray<FUML_PlayerStat> LocalStatsArray;
						for(const auto& StatsVar : UsersStatsResult)
						{
							for(auto StoredValueRef : StatsVar->Stats)
							{
								FString Keyname = StoredValueRef.Key;
								int32 Value;
								StoredValueRef.Value.GetValue(Value);
								FUML_PlayerStat LocalStats;
								LocalStats.StatName = Keyname;
								LocalStats.StatValue = FString::FromInt(Value);
								LocalStatsArray.Add(LocalStats);
							}
						}
						OnPlayerStatsRefreshed.ExecuteIfBound(LocalStatsArray);
					}
					else
					{
						UE_LOG(LogTemp,Warning,TEXT("Getting stats failed with error - %s"), *ResultState.ToLogString());
						OnPlayerStatsRefreshed.ExecuteIfBound(TArray<FUML_PlayerStat>());
					}
				}));
			}
		}
	}
}

void UUML_OnlineFunctions::SetPlayerStats(UObject* WorldContextObject, const FString& StatName, int32 StatValue)
{
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(const IOnlineIdentityPtr IdentityPointerRef = OnlineSub->GetIdentityInterface())
		{
			if(const IOnlineStatsPtr StatsPointerRef = OnlineSub->GetStatsInterface())
			{
				TArray<FOnlineStatsUserUpdatedStats> StatsArray;
				FOnlineStatsUserUpdatedStats StatsUserUpdated(IdentityPointerRef->GetUniquePlayerId(0).ToSharedRef());
				FOnlineStatUpdate StatUpdate;
				FOnlineStatValue StatValueVar;
				StatValueVar.SetValue(StatValue);
				StatUpdate.Set(StatValueVar, FOnlineStatUpdate::EOnlineStatModificationType::Sum);
				StatsUserUpdated.Stats.Add(StatName, StatUpdate);
				StatsArray.Add(StatsUserUpdated);
				StatsPointerRef->UpdateStats(IdentityPointerRef->GetUniquePlayerId(0).ToSharedRef(), StatsArray, FOnlineStatsUpdateStatsComplete::CreateLambda([](const FOnlineError& ResultState)
				{
					if(ResultState.WasSuccessful())
					{
						UE_LOG(LogTemp, Warning, TEXT("Stats updated successfully"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to update stats: %s"), *ResultState.ToLogString());
					}
				}));
			}
		}
	}
}

void UUML_OnlineFunctions::InviteUserToGame(UObject* WorldContextObject, const FUML_Friend& Friend)
{
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			if(!SessionPtr->GetNamedSession(NAME_PartySession))
			{
				UE_LOG(LogTemp, Warning, TEXT("No party session found"));
				return;
			}
			if(!Friend.UniqueNetId.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid friend ID"));
				return;
			}			
			const FUniqueNetIdRepl& UniqueNetIdRepl = Friend.UniqueNetId;
			if (!UniqueNetIdRepl.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid UniqueNetIdRepl"));
				return;
			}

			const FUniqueNetId* FriendId = UniqueNetIdRepl.GetUniqueNetId().Get();
			if (!FriendId)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid friend ID"));
				return;
			}

			// Proceed with sending session invite
			SessionPtr->SendSessionInviteToFriend(0, NAME_PartySession, *FriendId);
		}
	}
}

bool UUML_OnlineFunctions::IsInOnlineGameSession(UObject* WorldContextObject, FName SessionName)
{
	if(!WorldContextObject)
	{
		return false;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			return SessionPtr->GetNamedSession(SessionName) != nullptr;
		}
	}
	return false;
}

bool UUML_OnlineFunctions::IsFriendInOnlineGameSession(UObject* WorldContextObject, const FUniqueNetIdRepl& UniqueNetId)
{
	if(!WorldContextObject)
	{
		return false;
	}
	if(WorldContextObject->GetWorld() == nullptr)
	{
		return false;
	}
	AGameStateBase* GameState = WorldContextObject->GetWorld()->GetGameState();
	if(!GameState)
	{
		return false;
	}
	for(auto PlayerState : GameState->PlayerArray)
	{
		if(PlayerState->GetUniqueId() == UniqueNetId)
		{
			return true;
		}
	}
	return false;
}

FUniqueNetIdRepl UUML_OnlineFunctions::GetSessionOwner(UObject* WorldContextObject, FName SessionName, bool& bIsValid)
{
	bIsValid = false;
	if(!WorldContextObject)
	{
		return FUniqueNetIdRepl();
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface())
		{
			if(FOnlineSession* SessionInfo = SessionPtr->GetNamedSession(SessionName))
			{
				bIsValid = true;
				return SessionInfo->OwningUserId;
			}
		}
	}
	return FUniqueNetIdRepl();
}

bool UUML_OnlineFunctions::IsUniqueNetIdEqual(const FUniqueNetIdRepl& UniqueNetId1,
	const FUniqueNetIdRepl& UniqueNetId2)
{
	return UniqueNetId1 == UniqueNetId2;
}

bool UUML_OnlineFunctions::RequestPlayerProfileImage(UObject* WorldContextObject, const FUniqueNetIdRepl& UniqueNetId, UTexture2D* DefaultTexture, UTexture2D*& OutTexture, bool& RequestInProgress)
{
	RequestInProgress = false;
	if(!WorldContextObject)
	{
		return false;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld(), STEAM_SUBSYSTEM))
	{
#if USING_STEAM
		uint32 Width = 0;
	uint32 Height = 0;
	if (SteamAPI_Init())
	{
		if(SteamFriends())
		{
			int Picture = 0;
			// Convert the UniqueNetId to a int64
			FString UniqueNetIdString = UniqueNetId.GetUniqueNetId().Get()->ToString();
			uint64 LocalSteamID = FCString::Atoi64(*UniqueNetIdString);
			Picture = SteamFriends()->GetMediumFriendAvatar(LocalSteamID);
			if (Picture == -1)
			{
				SteamFriends()->RequestUserInformation(LocalSteamID, false);
				RequestInProgress = true;
				UE_LOG(LogTemp, Warning, TEXT("Failed to get steam avatar for %lld"), LocalSteamID);
				return false;
			}
			if(Picture == 0)
			{
				SteamFriends()->RequestUserInformation(LocalSteamID, false);
				UE_LOG(LogTemp, Warning, TEXT("No steam avatar for %lld"), LocalSteamID);
				return false;
			}
			if(!SteamUtils()->GetImageSize(Picture, &Width, &Height))
			{
				UE_LOG(LogTemp, Warning, TEXT("GetImageSize failed for %lld"), LocalSteamID);
				return false;
			}
			if (Width > 0 && Height > 0)
			{
				//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
				uint8 *oAvatarRGBA = new uint8[Width * Height * 4];


				//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
				SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));
			
				UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
				// Switched to a Memcpy instead of byte by byte transer
				uint8* MipData = (uint8*)Avatar->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
				Avatar->GetPlatformData()->Mips[0].BulkData.Unlock();

				// Original implementation was missing this!!
				// the hell man......
				delete[] oAvatarRGBA;

				//Setting some Parameters for the Texture and finally returning it
				Avatar->GetPlatformData()->SetNumSlices(1);
				Avatar->NeverStream = true;
				//Avatar->CompressionSettings = TC_EditorIcon;

				Avatar->UpdateResource();
				OutTexture = Avatar;
				return true;
			}
			UE_LOG(LogTemp, Warning, TEXT("Bad Height / Width with steam avatar!"));
			return false;
		}
		UE_LOG(LogTemp, Warning, TEXT("SteamFriends() failed"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("SteamAPI_Init() failed"));
		return false;
#else
		OutTexture = DefaultTexture;
		return false;
#endif
	}
	OutTexture = DefaultTexture;
	return false;
}

void UUML_OnlineFunctions::GetLeaderboard(UObject* WorldContextObject, FName LeaderboardName, int32 Rank, int32 Range, FUML_OnLeaderboardRefreshed OnLeaderboardRefreshed)
{
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface())
		{
			FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
			ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5
			FString LeaderboardNameString = LeaderboardName.ToString();
			ReadRef->LeaderboardName = LeaderboardNameString;
#else
			ReadRef->LeaderboardName = LeaderboardName;
#endif
			Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(FOnLeaderboardReadComplete::FDelegate::CreateLambda([LeaderboardName, ReadRef, OnLeaderboardRefreshed](bool bWasSuccessful)
			{
				if(bWasSuccessful)
				{
					TArray<FUML_LeaderboardEntry> LeaderboardEntries;
					for (auto Row : ReadRef->Rows)
					{
						FUML_LeaderboardEntry Entry;
						Entry.Rank = Row.Rank;
						Entry.PlayerName = Row.NickName;
						int32 Score = 0;
						UE_LOG(LogTemp, Warning, TEXT("Row Rank: %d"), Row.Rank);
#if USING_STEAM_INTEGRATION_KIT || USING_ONLINESUBSYSTEMSTEAM
						if (Row.Columns.Num() > 0)
						{
							Row.Columns[TEXT("Score")].GetValue(Score);
						}
#else
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5
if (Row.Columns.Num() > 0)
{
	const FString& FirstKey = Row.Columns.begin()->Key;
	Row.Columns.Find(FirstKey)->GetValue(Score);
}
#else
Row.Columns.Find("None")->GetValue(Score);
#endif
#endif
						Entry.Score = Score;
						LeaderboardEntries.Add(Entry);
					}
					OnLeaderboardRefreshed.ExecuteIfBound(LeaderboardEntries);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to read leaderboard: %s"), *LeaderboardName.ToString());
					OnLeaderboardRefreshed.ExecuteIfBound(TArray<FUML_LeaderboardEntry>());
				}
				
			}));
			Leaderboards->ReadLeaderboardsAroundRank(Rank, Range,ReadRef);
		}
	}
}

void UUML_OnlineFunctions::GetAchievements(UObject* WorldContextObject,
	FUML_OnAchievementsRefreshed OnAchievementsRefreshed)
{
	if(!WorldContextObject)
	{
		return;
	}
	if(IOnlineSubsystem* OnlineSub = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if(IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface())
		{
			if(const IOnlineIdentityPtr IdentityPtrRef = OnlineSub->GetIdentityInterface())
			{
				Achievements->QueryAchievements(*IdentityPtrRef->GetUniquePlayerId(0), FOnQueryAchievementsCompleteDelegate::CreateLambda([OnAchievementsRefreshed, Achievements, IdentityPtrRef](const FUniqueNetId& UniqueNetId, bool bWasSuccess)
				{
					if(bWasSuccess)
					{
						Achievements->QueryAchievementDescriptions(*IdentityPtrRef->GetUniquePlayerId(0), FOnQueryAchievementsCompleteDelegate::CreateLambda([OnAchievementsRefreshed, Achievements](const FUniqueNetId& UniqueNetId, bool bWasSuccess)
						{
							if(bWasSuccess)
							{
								TArray<FUML_Achievement> AchievementsArray;
								TArray<FOnlineAchievement> CachedAchievements;
								Achievements->GetCachedAchievements(UniqueNetId, CachedAchievements);
								FOnlineAchievementDesc CachedAchievementDesc;
								for(auto Achievement : CachedAchievements)
								{
									Achievements->GetCachedAchievementDescription(Achievement.Id, CachedAchievementDesc);
									FUML_Achievement LocalAchievement;
									LocalAchievement.Id = Achievement.Id;
									LocalAchievement.Progress = Achievement.Progress;
									LocalAchievement.Title = CachedAchievementDesc.Title;
									LocalAchievement.LockedDesc = CachedAchievementDesc.LockedDesc;
									LocalAchievement.UnlockedDesc = CachedAchievementDesc.UnlockedDesc;
									LocalAchievement.bIsHidden = CachedAchievementDesc.bIsHidden;
									LocalAchievement.UnlockTime = CachedAchievementDesc.UnlockTime;
									AchievementsArray.Add(LocalAchievement);
								}
								UE_LOG(LogTemp, Warning, TEXT("Achievements read successfully with %d achievements"), AchievementsArray.Num());
								OnAchievementsRefreshed.ExecuteIfBound(AchievementsArray);
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Failed to get achievement descriptions"));
								OnAchievementsRefreshed.ExecuteIfBound(TArray<FUML_Achievement>());
							}
						}));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to get achievements"));
						OnAchievementsRefreshed.ExecuteIfBound(TArray<FUML_Achievement>());
					}
				}));
			}
		};
	}
}

void UUML_OnlineFunctions::QueryOffers(UObject* WorldContextObject, FUML_OnOffersRefreshed OnOffersRefreshed)
{
#if USING_STEAM_INTEGRATION_KIT || USING_ONLINESUBSYSTEMSTEAM
	UE_LOG(LogTemp, Warning, TEXT("Steam doesn't support querying offers directly. Please use the Steamworks SDK OR Steam Integration Kit to query offers"));
	OnOffersRefreshed.ExecuteIfBound(false, TArray<FUML_OffersStruct>());
#else
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if (const IOnlineStoreV2Ptr StoreV2Ptr = SubsystemRef->GetStoreV2Interface())
		{
			if (const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
			{
				FOnQueryOnlineStoreOffersComplete OnQueryOnlineStoreOffersComplete;
				OnQueryOnlineStoreOffersComplete.BindLambda([StoreV2Ptr, WorldContextObject, OnOffersRefreshed](
					bool bWasSuccessful,
					const TArray<FUniqueOfferId>& OfferIds,
					const FString& Error)
					{
						if (bWasSuccessful && StoreV2Ptr.IsValid())
						{
							TArray<FOnlineStoreOfferRef> Offers;
							StoreV2Ptr->GetOffers(Offers);
							TArray<FUML_OffersStruct> OfferArray;
							for (int32 i = 0; i < Offers.Num(); ++i)
							{
								OfferArray[i].ItemID = Offers[i]->OfferId;
								OfferArray[i].ItemName = Offers[i]->Title;
								OfferArray[i].Description = Offers[i]->Description;
								OfferArray[i].ExpirationDate = Offers[i]->ExpirationDate;
								OfferArray[i].LongDescription = Offers[i]->LongDescription;
								OfferArray[i].NumericPrice = Offers[i]->NumericPrice;
								OfferArray[i].PriceText = Offers[i]->PriceText;
								OfferArray[i].RegularPrice = Offers[i]->RegularPrice;
								OfferArray[i].ReleaseDate = Offers[i]->ReleaseDate;
								OfferArray[i].RegularPriceText = Offers[i]->RegularPriceText;
							}
							OnOffersRefreshed.ExecuteIfBound(true, OfferArray);
						}
						else
						{
							OnOffersRefreshed.ExecuteIfBound(false, TArray<FUML_OffersStruct>());
						}
					});
				StoreV2Ptr->QueryOffersByFilter(*IdentityPointerRef->GetUniquePlayerId(0).Get(), FOnlineStoreFilter(), OnQueryOnlineStoreOffersComplete);
			}
			else
			{
				OnOffersRefreshed.ExecuteIfBound(false, 	TArray<FUML_OffersStruct>());
			}
		}
		else
		{
			OnOffersRefreshed.ExecuteIfBound(false, 	TArray<FUML_OffersStruct>());
		}
	}
	else
	{
		OnOffersRefreshed.ExecuteIfBound(false, 	TArray<FUML_OffersStruct>());
	}
#endif
}

void UUML_OnlineFunctions::PurchaseOffer(UObject* WorldContextObject, const FString& OfferId,
	const FUML_OnStorePurchaseComplete& OnPurchaseComplete)
{
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if (const IOnlinePurchasePtr PurchasePtr = SubsystemRef->GetPurchaseInterface())
		{
			if (const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
			{
				FPurchaseCheckoutRequest Request = {};
				Request.AddPurchaseOffer(TEXT(""), OfferId, 1);

				PurchasePtr->Checkout(*IdentityPointerRef->GetUniquePlayerId(0).Get(),
					Request,
					FOnPurchaseCheckoutComplete::CreateLambda(
						[WorldContextObject, OnPurchaseComplete](
						const FOnlineError& Result,
						const TSharedRef<FPurchaseReceipt>& Receipt)
						{
							if (Result.WasSuccessful())
							{
								OnPurchaseComplete.ExecuteIfBound(true, Receipt->TransactionId);
							}
							else
							{
								OnPurchaseComplete.ExecuteIfBound(false, Result.ToLogString());
							}
						})
				);
			}
			else
			{
				OnPurchaseComplete.ExecuteIfBound(false, TEXT("No identity interface"));
			}
		}
		else
		{
			OnPurchaseComplete.ExecuteIfBound(false, TEXT("No purchase interface"));
		}
	}
	else
	{
		OnPurchaseComplete.ExecuteIfBound(false, TEXT("No online subsystem"));
	}
}

void UUML_OnlineFunctions::GetOwnedItems(UObject* WorldContextObject, const FUM_OnInventoryRefreshed& OnOwnedItemsRefreshed)
{
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(WorldContextObject->GetWorld()))
	{
		if (const IOnlineStoreV2Ptr StoreV2Ptr = SubsystemRef->GetStoreV2Interface())
		{
			if (const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
			{
				if (const IOnlinePurchasePtr Purchase = SubsystemRef->GetPurchaseInterface())
				{
					Purchase->QueryReceipts(*IdentityPointerRef->GetUniquePlayerId(0).Get(), false,
										   FOnQueryReceiptsComplete::CreateLambda(
											   [WorldContextObject, SubsystemRef, IdentityPointerRef, Purchase, OnOwnedItemsRefreshed
											   ](const FOnlineError& Error)
											   {
												   if (Error.WasSuccessful())
												   {
													   if (Purchase)
													   {
														   TArray<FString> ItemNames;
														   TArray<FPurchaseReceipt> Receipts;
														   Purchase->GetReceipts(*IdentityPointerRef->GetUniquePlayerId(0).Get(), Receipts);
														   for (int i = 0; i < Receipts.Num(); i++)
														   {
															   ItemNames.Add(Receipts[i].ReceiptOffers[0].LineItems[0].ItemName);
														   }
														   OnOwnedItemsRefreshed.ExecuteIfBound(true, ItemNames);
													   }
													   else
													   {
														   OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
													   }
												   }
												   else
												   {
													   UE_LOG(LogTemp, Warning, TEXT("Failed to query receipts: %s"), *Error.ToLogString());
												   OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
												   }
											   }));
				}
				else
				{
					OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
				}
			}
			else
			{
				OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
			}
		}
		else
		{
			OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
		}
	}
	else
	{
		OnOwnedItemsRefreshed.ExecuteIfBound(false, TArray<FString>());
	}
}

bool UUML_OnlineFunctions::IsRunningSteamSubsystem(UObject* WorldContextObject)
{
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(WorldContextObject->GetWorld(), STEAM_SUBSYSTEM))
	{
		return true;
	}
	return false;
}