// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    //duream code edit collision setting.
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
    RocketMovementComponent->bRotationFollowsVelocity = true;
    RocketMovementComponent->SetIsReplicated(true);
}

#if WITH_EDITOR
void AProjectileRocket::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
    Super::PostEditChangeProperty(Event);

    FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileRocket, InitialSpeed))
    {
        if (RocketMovementComponent)
        {
            RocketMovementComponent->InitialSpeed = InitialSpeed;
            RocketMovementComponent->MaxSpeed = InitialSpeed;
        }
    }
}
#endif

void AProjectileRocket::BeginPlay()
{
    Super::BeginPlay();

    SpawnTrailSystem();

    if (ProjectileLoop && LoopingSoundAttenuation)
    {
        ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
            ProjectileLoop,
            GetRootComponent(),
            FName(),
            GetActorLocation(),
            EAttachLocation::KeepWorldPosition,
            false,
            1.f,
            1.f,
            0.f,
            LoopingSoundAttenuation,
            (USoundConcurrency*)nullptr,
            false
        );
    }
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == GetOwner() || OtherActor == GetInstigator())
    {
        return;
    }

    if (HasAuthority())
    {
        ExplodeDamage();
        Destroy();
    }
    else if (bUseServerSideRewind)
    {
        Destroy();
    }
}

void AProjectileRocket::Destroyed()
{
    if (ImpactParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
    }
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }

    if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
    {
        ProjectileLoopComponent->Stop();
    }

    AActor::Destroyed();
}