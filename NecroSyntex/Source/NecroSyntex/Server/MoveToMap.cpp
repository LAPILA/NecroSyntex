// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToMap.h"
#include "Components/BoxComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"

// Sets default values
AMoveToMap::AMoveToMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	movePoint = CreateDefaultSubobject<UBoxComponent>(TEXT("movePoint"));
	movePoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMoveToMap::BeginPlay()
{
	Super::BeginPlay();
	movePoint->OnComponentBeginOverlap.AddDynamic(this, &AMoveToMap::BoxBeginOverlap);
}

void AMoveToMap::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
	//if (HasAuthority() && player) {
	//	AGameStateBase* gameState = GetWorld()->GetGameState();
	//	if (gameState) {
	//		TArray<APlayerState*> PlayerArray = gameState->PlayerArray;
	//		
	//		for (auto playerState : PlayerArray) {
	//			if (playerState) {
	//				APlayerController* playerController = playerState->GetOwnerPlayerController();
	//				if (necroPlayerController) {
	//					//NS Node need..
	//				}
	//			}
	//		}
	//		GetWorld()->ServerTravel(TEXT("bossmap4?listen"));
	//	}
	//}
}

// Called every frame
//void AMoveToMap::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

