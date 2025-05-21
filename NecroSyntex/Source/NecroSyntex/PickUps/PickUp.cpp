// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "Kismet/GameplayStatics.h"
#include "Sound\SoundCue.h"
#include "Components\SphereComponent.h"
#include "NecroSyntex\Weapon\WeaponTypes.h"

// Sets default values
APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.0f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRenderCustomDepth(false);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);

}

void APickUp::BeginPlay()
{
	Super::BeginPlay();
	
	/*PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);*/
	
	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnSphereOverlap);
	}
}

void APickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBoxIndex, bool bFromSweap, const FHitResult& SweepResult)
{

}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh)
	{
		//회전 효과 싫으면 제거
		PickupMesh->AddLocalRotation(FRotator(0.0f, BaseTurnRate * DeltaTime, 0.f));
	}
}

void APickUp::Destroyed()
{
	Super::Destroy();

	//PickUpSound
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}


}

