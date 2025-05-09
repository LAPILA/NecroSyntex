// Fill out your copyright notice in the Description page of Project Settings.


#include "NecroSyntexHud.h"
#include "GameFramework/PlayerController.h"
#include "NecroSyntex/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterOverlay.h"
#include "NecroSyntex\HUD\BuffDebuffContainerWidget.h"
#include "NecroSyntex\HUD\BuffDebuffWidget.h"
#include "Announcement.h"

void ANecroSyntexHud::BeginPlay()
{
	Super::BeginPlay();
}

void ANecroSyntexHud::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		if (CharacterOverlay)
		{
			CharacterOverlay->AddToViewport(-1);
			UE_LOG(LogTemp, Log, TEXT("CharacterOverlay successfully added to viewport"));
		}
	}
	/*
	//TEST CODE
	if (CharacterOverlay && CharacterOverlay->BuffDebuffContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffDebuffContainer is valid. Running test buffs."));

		// Buff 1 - Speed
		FBuffDebuffInfo SpeedBuff;
		SpeedBuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icon_accuracy/2_accuracy"));
		SpeedBuff.Duration = 30.f;
		SpeedBuff.bIsBuff = true;
		CharacterOverlay->BuffDebuffContainer->CreateEntryWidget("SpeedBuff", SpeedBuff);

		// Buff 2 - Regen
		FBuffDebuffInfo RegenBuff;
		RegenBuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icon_power/9_power"));
		RegenBuff.Duration = 20.f;
		RegenBuff.bIsBuff = true;
		CharacterOverlay->BuffDebuffContainer->CreateEntryWidget("RegenBuff", RegenBuff);

		// Debuff 1 - Poison
		FBuffDebuffInfo PoisonDebuff;
		PoisonDebuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icon_potion/7_potion"));
		PoisonDebuff.Duration = 30.f;
		PoisonDebuff.bIsBuff = false;
		CharacterOverlay->BuffDebuffContainer->CreateEntryWidget("Poison", PoisonDebuff);

		// Debuff 2 - Slow
		FBuffDebuffInfo SlowDebuff;
		SlowDebuff.Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/icons/icon_accuracy/11_accuracy"));
		SlowDebuff.Duration = 15.f;
		SlowDebuff.bIsBuff = false;
		CharacterOverlay->BuffDebuffContainer->CreateEntryWidget("Slow", SlowDebuff);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BuffDebuffContainer is null, cannot run test."));
	}
	*/
}

void ANecroSyntexHud::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void ANecroSyntexHud::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}
void ANecroSyntexHud::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}