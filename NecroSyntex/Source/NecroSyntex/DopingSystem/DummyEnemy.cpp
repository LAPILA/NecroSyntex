// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "NecroSyntex/Weapon/ProjectileBullet.h"

// Sets default values
ADummyEnemy::ADummyEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(CapsuleComp);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionObjectType(ECC_Pawn); // Pawn 타입으로 설정
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore); // 기본적으로 모든 충돌 무시
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 벽 같은 오브젝트와 충돌
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CapsuleComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);


}

// Called when the game starts or when spawned
void ADummyEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADummyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADummyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADummyEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("적이 총알을 맞음!"));
	if (OtherActor->IsA(AProjectileBullet::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("적이 총알을 맞음!"));
	}
}

float ADummyEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("적이 총알을 맞음!"));

	return 0;
}