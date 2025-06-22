// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionTrigger.h"
#include "Components/BoxComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NecroSyntex/GameMode/NecroSyntexGameMode.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMissionTrigger::AMissionTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetLoadOnClient = true; // 클라이언트에서 객체를 로드하도록 설정

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	MissionTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MissionTrigger"));
	MissionTriggerBox->SetupAttachment(RootComponent);
	MissionTriggerBox->SetRelativeLocation(FVector(168.f, 0.f, 0.f));
	MissionTriggerBox->SetBoxExtent(FVector(100.f, 50.f, 50.f));
	MissionTriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	MissionTriggerBox->SetGenerateOverlapEvents(true);

	MissionTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MissionTriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	MissionTriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	MissionTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MissionTriggerBox->SetGenerateOverlapEvents(true);

	MissionDuration = 50.0f;
	MissionRegion = "Survival";
	MissionName = "Survival";
	OnTrigger = false;
}

void AMissionTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMissionTrigger, OnTrigger);
	DOREPLIFETIME(AMissionTrigger, OnTriggerTimer);
}

// Called when the game starts or when spawned
void AMissionTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (MissionTriggerBox) {
		MissionTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMissionTrigger::OnBoxTriggerOverlapBegin);
		MissionTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMissionTrigger::OnBoxTriggerOverlapEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SkillAttackArea is nullptr"));
	}

	if (!HasAuthority()) {
		if (IsValid(this)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Client sees the Actor!"));
		}
	}
	
}

// Called every frame
void AMissionTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMissionTrigger::OnBoxTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Server : In``"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client : In``"));
	}

	if (!HasAuthority()) return;


	if (!OnTrigger) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerInTrigger++;

		ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
		if (GS && !GS->OngoingMission)
		{

			if (GS->TotalPlayer == PlayerInTrigger)
			{
				if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
				{
					//시작할 미션 설정
					GM->MissionManager->MissionSet(MissionName, MissionRegion, MissionDuration);
					GM->MissionManager->CMTSet(this);

					//미션 시작 카운터 다운 시작 (도중에 영역 나가면 타이머 취소 및 초기화)
					GM->MissionManager->MissionCountdownStart();

					/*GetWorld()->GetTimerManager().SetTimer(
						TriggerTimer,
						[this]() { TriggerDestroy(); },
						GM->MissionManager->count + 1.0f,
						false
					);*/

					TriggerMakeNoise();
					OnTriggerTimer = true;

				}
			}
		}
	}
}

void AMissionTrigger::OnBoxTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;


	//if (!OnTriggerTimer) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerInTrigger--;
		ANecroSyntexGameState* GS = Cast<ANecroSyntexGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			if (GS->TotalPlayer > PlayerInTrigger)
			{
				if (GS->MissionCountDownBool == true) {
					if (ANecroSyntexGameMode* GM = Cast<ANecroSyntexGameMode>(UGameplayStatics::GetGameMode(this)))
					{
						GM->MissionManager->MissionCountdownCancel();
						//GetWorld()->GetTimerManager().ClearTimer(TriggerTimer);
						OnTriggerTimer = false;
					}
				}
			}
		}
	}
}

void AMissionTrigger::MissionTriggerActivate_Implementation()
{
	OnTrigger = false;
}

void AMissionTrigger::TriggerDestroy()
{
	OnTrigger = false;

	Destroy();

}

void AMissionTrigger::MultiCastLog_Implementation()
{
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client : Multicast!!!!!``"));
	}
}

//void AMissionTrigger::DestroyClient_Implementation()
//{
//	Destroy();
//}