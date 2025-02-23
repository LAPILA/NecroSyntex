// Fill out your copyright notice in the Description page of Project Settings.

#include "NecroSyntexPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NecroSyntex/HUD/NecroSyntexHud.h"
#include "NecroSyntex/HUD/CharacterOverlay.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/PlayerState/NecroSyntexPlayerState.h"
#include "NecroSyntex/HUD/Announcement.h"
#include "NecroSyntex\NecroSyntaxComponents\CombatComponent.h"
#include "Kismet/GameplayStatics.h"

void ANecroSyntexPlayerController::BeginPlay()
{
	Super::BeginPlay();
	NecroSyntexHUD = Cast<ANecroSyntexHud>(GetHUD());
	ServerCheckMatchState();
}

void ANecroSyntexPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANecroSyntexPlayerController, MatchState);
}

void ANecroSyntexPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void ANecroSyntexPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}


void ANecroSyntexPlayerController::ServerCheckMatchState_Implementation()
{
	ANecroSyntexGameMode* GameMode = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmUpTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartingTime);
	}
}
void ANecroSyntexPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (NecroSyntexHUD && MatchState == MatchState::WaitingToStart)
	{
		NecroSyntexHUD->AddAnnouncement();
	}
}


//Player State HUD
void ANecroSyntexPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->HealthBar;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		UMaterialInstanceDynamic* HealthBarMaterial = Cast<UMaterialInstanceDynamic>(NecroSyntexHUD->CharacterOverlay->HealthBar->GetDynamicMaterial());
		if (HealthBarMaterial)
		{
			HealthBarMaterial->SetScalarParameterValue(FName("Percentage"), HealthPercent);
		}
	}
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}
void ANecroSyntexPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->ShieldBar;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		UMaterialInstanceDynamic* ShieldBarMaterial = Cast<UMaterialInstanceDynamic>(NecroSyntexHUD->CharacterOverlay->ShieldBar->GetDynamicMaterial());
		if (ShieldBarMaterial)
		{
			ShieldBarMaterial->SetScalarParameterValue(FName("Percentage"), ShieldPercent);
		}
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void ANecroSyntexPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	if (PlayerCharacter)
	{
		SetHUDHealth(PlayerCharacter->GetHealth(), PlayerCharacter->GetMaxHealth());
		SetHUDShield(PlayerCharacter->GetShield(), PlayerCharacter->GetMaxShield());
	}
}

//Weapon State HUD
void ANecroSyntexPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		NecroSyntexHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ANecroSyntexPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("/ %d"), Ammo);
		NecroSyntexHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}


//Player Score HUD
void ANecroSyntexPlayerController::SetHUDScore(float Score)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("Army Score : %d"), FMath::FloorToInt(Score));
		NecroSyntexHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}
}

void ANecroSyntexPlayerController::SetHUDDefeats(int32 Defeats)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->DieAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("Die Count : %d"), Defeats);
		NecroSyntexHUD->CharacterOverlay->DieAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

//Game State HUD
void ANecroSyntexPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Hours = FMath::FloorToInt(CountdownTime / 3600.0f);
		int32 Minutes = FMath::FloorToInt((CountdownTime - Hours * 3600.0f) / 60.0f);
		int32 Seconds = FMath::FloorToInt(CountdownTime - (Hours * 3600.0f + Minutes * 60.0f));
		int32 Milliseconds = FMath::RoundToInt((CountdownTime - FMath::FloorToFloat(CountdownTime)) * 1000.0f);

		FString CountdownText = FString::Printf(TEXT("%02d:%02d:%02d:%03d"), Hours, Minutes, Seconds, Milliseconds);

		NecroSyntexHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ANecroSyntexPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->Announcement &&
		NecroSyntexHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		int32 Hours = FMath::FloorToInt(CountdownTime / 3600.0f);
		int32 Minutes = FMath::FloorToInt((CountdownTime - Hours * 3600.0f) / 60.0f);
		int32 Seconds = FMath::FloorToInt(CountdownTime - (Hours * 3600.0f + Minutes * 60.0f));
		int32 Milliseconds = FMath::RoundToInt((CountdownTime - FMath::FloorToFloat(CountdownTime)) * 1000.0f);

		FString CountdownText = FString::Printf(TEXT("%02d:%02d:%02d:%03d"), Hours, Minutes, Seconds, Milliseconds);

		NecroSyntexHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ANecroSyntexPlayerController::SetHUDGrenades(int32 Grenades)
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	bool bHUDValid = NecroSyntexHUD &&
		NecroSyntexHUD->CharacterOverlay &&
		NecroSyntexHUD->CharacterOverlay->GrenadesText;
	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		NecroSyntexHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}

void ANecroSyntexPlayerController::SetHUDTime()
{
	float CurrentTime = MatchTime - GetServerTime();
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	float Milliseconds = FMath::Frac(CurrentTime) * 1000.0f;

	if (CountdownInt != SecondsLeft || PreviousMilliseconds != (int)Milliseconds)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
	PreviousMilliseconds = (int)Milliseconds;
}

void ANecroSyntexPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (NecroSyntexHUD && NecroSyntexHUD->CharacterOverlay)
		{
			CharacterOverlay = NecroSyntexHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
				if (PlayerCharacter && PlayerCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(PlayerCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void ANecroSyntexPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}
void ANecroSyntexPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}
float ANecroSyntexPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}
void ANecroSyntexPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ANecroSyntexPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}
void ANecroSyntexPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ANecroSyntexPlayerController::HandleMatchHasStarted()
{
	NecroSyntexHUD = NecroSyntexHUD == nullptr ? Cast<ANecroSyntexHud>(GetHUD()) : NecroSyntexHUD;
	if (NecroSyntexHUD)
	{
		NecroSyntexHUD->AddCharacterOverlay();
		if (NecroSyntexHUD->Announcement)
		{
			NecroSyntexHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}