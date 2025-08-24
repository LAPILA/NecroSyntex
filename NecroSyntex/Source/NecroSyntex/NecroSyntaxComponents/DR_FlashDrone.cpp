// Copyright NecroSyntex. All Rights Reserved.

#include "DR_FlashDrone.h"
#include "GameFramework/Pawn.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ADR_FlashDrone::ADR_FlashDrone()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicateMovement(false);

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    RootComponent = CollisionComp;
    CollisionComp->InitSphereRadius(35.f);
    CollisionComp->SetCollisionProfileName(TEXT("Pawn"));
    CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    DroneMesh->SetupAttachment(RootComponent);
    DroneMesh->SetCastShadow(false);
    DroneMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(DroneMesh);
    SpotLight->Intensity = 50000.f;
    SpotLight->SetRelativeLocation(FVector(0, 0, 30.f));
}

void ADR_FlashDrone::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(
            TeleportCheckTimer, this,
            &ADR_FlashDrone::CheckDistanceAndTeleport,
            CheckInterval, true);
    }
}

void ADR_FlashDrone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADR_FlashDrone, CurrentAimTarget);
    DOREPLIFETIME(ADR_FlashDrone, ReplicatedLocation);
    DOREPLIFETIME(ADR_FlashDrone, ReplicatedRotation);
    DOREPLIFETIME(ADR_FlashDrone, TargetActor);
}

void ADR_FlashDrone::InitFollowing(AActor* InTarget, float InMaxDist)
{
    TargetActor = InTarget;
    MaxDistance = InMaxDist;
    InterpolationTargetLocation = GetActorLocation();
    InterpolationTargetRotation = GetActorRotation();
}

void ADR_FlashDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsValid(TargetActor)) return;

    if (HasAuthority())
    {
        // 1. 목표 위치 계산
        const APawn* OwningPawn = Cast<APawn>(TargetActor);
        FVector Base = TargetActor->GetActorLocation() + TargetActor->GetActorRightVector() * PivotRightOffset + FVector(0, 0, OrbitHeight);
        const bool bNowAiming = CurrentAimTarget.SizeSquared() > 1.f;
        FVector DesiredLoc;
        if (bNowAiming)
        {
            const FVector AimDir = (CurrentAimTarget - Base).GetSafeNormal();
            const FVector Right = FVector::CrossProduct(FVector::UpVector, AimDir).GetSafeNormal();
            const FVector Up = FVector::CrossProduct(AimDir, Right);
            DesiredLoc = Base + AimDir * AimOffset.X + Right * AimOffset.Y + Up * AimOffset.Z;
        }
        else
        {
            OrbitYaw = FMath::Fmod(OrbitYaw + OrbitSpeed * DeltaTime, 360.f);
            const float Rad = FMath::DegreesToRadians(OrbitYaw);
            DesiredLoc = Base + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0) * OrbitRadius;
        }

        // 2. 서버 측 드론 위치 이동
        FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLoc, DeltaTime, FollowInterpSpeed);
        FHitResult HitResult;
        SetActorLocation(NewLocation, true, &HitResult);
        if (HitResult.bBlockingHit) { SetActorLocation(HitResult.Location); }

        // 3. 서버 측 드론 회전 계산 및 적용
        FVector Dir;
        if (bNowAiming) {
            Dir = (CurrentAimTarget - SpotLight->GetComponentLocation()).GetSafeNormal();
        }
        else {
            if (OwningPawn && OwningPawn->GetController()) {
                Dir = OwningPawn->GetController()->GetControlRotation().Vector();
            }
            else {
                Dir = TargetActor->GetActorForwardVector();
            }
        }
        const float TurnSpd = bNowAiming ? 20.f : 10.f;
        FRotator TargetRotation = Dir.Rotation();
        SpotLight->SetWorldRotation(FMath::RInterpTo(SpotLight->GetComponentRotation(), TargetRotation, DeltaTime, TurnSpd));

        // 4. 최종 위치와 회전 값을 복제 변수에 저장
        ReplicatedLocation = GetActorLocation();
        ReplicatedRotation = SpotLight->GetComponentRotation();
    }
    else
    {
        // 클라이언트는 서버가 보내준 위치와 회전 값을 향해 부드럽게 보간합니다.
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), InterpolationTargetLocation, DeltaTime, FollowInterpSpeed));
        SpotLight->SetWorldRotation(FMath::RInterpTo(SpotLight->GetComponentRotation(), InterpolationTargetRotation, DeltaTime, FollowInterpSpeed));
    }
}

void ADR_FlashDrone::OnRep_ServerState()
{
    InterpolationTargetLocation = ReplicatedLocation;
    InterpolationTargetRotation = ReplicatedRotation;
}

void ADR_FlashDrone::SetAimTarget(const FVector& NewTarget)
{
    // 클라이언트는 계산 없이 서버에 조준 위치만 보고합니다.
    if (!HasAuthority())
    {
        ServerSetAimTarget(NewTarget);
    }
}

void ADR_FlashDrone::ServerSetAimTarget_Implementation(const FVector_NetQuantize& NewTarget)
{
    CurrentAimTarget = NewTarget;
}


void ADR_FlashDrone::CheckDistanceAndTeleport() { if (TargetActor && FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation()) > FMath::Square(MaxDistance)) { ForceTeleportToTarget(); } }

void ADR_FlashDrone::ForceTeleportToTarget()
{
    if (HasAuthority() && IsValid(TargetActor))
    {
        const FVector TeleportLocation = TargetActor->GetActorLocation() + FVector(0, 0, OrbitHeight);
        const FRotator TeleportRotation = TargetActor->GetActorRotation();
        Multicast_ForceTeleport(TeleportLocation, TeleportRotation);
    }
}

void ADR_FlashDrone::Multicast_ForceTeleport_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
    SetActorLocation(NewLocation);
    SpotLight->SetWorldRotation(NewRotation);
    InterpolationTargetLocation = NewLocation;
    InterpolationTargetRotation = NewRotation;
}