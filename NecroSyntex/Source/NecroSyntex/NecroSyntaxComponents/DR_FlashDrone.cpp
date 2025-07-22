// DR_FlashDrone.cpp
#include "DR_FlashDrone.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

ADR_FlashDrone::ADR_FlashDrone()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicateMovement(true);

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = RootComp;

    DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    DroneMesh->SetupAttachment(RootComponent);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(DroneMesh);
    SpotLight->Intensity = 50'000.f;
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

void ADR_FlashDrone::InitFollowing(AActor* InTarget, float InMaxDist)
{
    TargetActor = InTarget;
    MaxDistance = InMaxDist;
}

void ADR_FlashDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!TargetActor) return;

    /* ─── 1) 위치 계산 ─── */
    FVector Base =
        TargetActor->GetActorLocation()
        + TargetActor->GetActorRightVector() * PivotRightOffset
        + FVector(0, 0, OrbitHeight);
    const bool bNowAiming = CurrentAimTarget.SizeSquared() > 1.f;

    FVector DesiredLoc;
    if (bNowAiming)
    {
        const FVector AimDir = (CurrentAimTarget - Base).GetSafeNormal();
        const FVector Right = FVector::CrossProduct(FVector::UpVector, AimDir).GetSafeNormal();
        const FVector Up = FVector::CrossProduct(AimDir, Right);

        DesiredLoc = Base
            + AimDir * AimOffset.X     // 앞/뒤
            + Right * AimOffset.Y     // 오른쪽
            + Up * AimOffset.Z;    // 위/아래  ← **피치에 반응**
    }
    else
    {
        OrbitYaw = FMath::Fmod(OrbitYaw + OrbitSpeed * DeltaTime, 360.f);
        const float Rad = FMath::DegreesToRadians(OrbitYaw);
        DesiredLoc = Base + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0) * OrbitRadius;
    }

    SetActorLocation(FMath::VInterpTo(GetActorLocation(), DesiredLoc, DeltaTime, FollowInterpSpeed));

    /* ─── 2) 밝기 감지 ─── */
    UpdateAutoLight();

    /* ─── 3) 라이트 방향 ─── */
    UpdateLightDirection();
}

void ADR_FlashDrone::UpdateAutoLight()
{
    if (!SpotLight || !TargetActor) return;

    FHitResult Hit;
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0, 0, 250.f);

    FCollisionQueryParams P; P.AddIgnoredActor(this); P.AddIgnoredActor(TargetActor);
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, P);

    const float Ambient = bHit ? 0.f : 1.f;      // 예시 로직
    SetLightActive(Ambient < 0.25f);
}

void ADR_FlashDrone::CheckDistanceAndTeleport()
{
    if (!TargetActor) return;
    if (FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation()) > FMath::Square(MaxDistance))
    {
        ForceTeleportToTarget();
    }
}

void ADR_FlashDrone::UpdateLightDirection()
{
    if (!SpotLight) return;

    const bool bNowAiming = CurrentAimTarget.SizeSquared() > 1.f;
    const FVector Start = GetActorLocation();
    const FVector Dir = bNowAiming ?
        (CurrentAimTarget - Start).GetSafeNormal() :
        (TargetActor->GetActorForwardVector() + FVector(0, 0, -0.18f)).GetSafeNormal();

    const float TurnSpd = bNowAiming ? 20.f : 6.f;
    const FRotator Target = Dir.Rotation();
    SpotLight->SetWorldRotation(FMath::RInterpTo(
        SpotLight->GetComponentRotation(), Target,
        GetWorld()->GetDeltaSeconds(), TurnSpd));
}

void ADR_FlashDrone::SetLightActive(bool bActive)
{
    if (SpotLight) SpotLight->SetVisibility(bActive);
}

void ADR_FlashDrone::ForceTeleportToTarget()
{
    if (TargetActor)
        SetActorLocation(TargetActor->GetActorLocation() + FVector(0, 0, OrbitHeight));
}

void ADR_FlashDrone::SetAimTarget(const FVector& NewTarget)
{
    if (HasAuthority())
    {
        CurrentAimTarget = NewTarget;
    }
    else
    {
        CurrentAimTarget = NewTarget;
        ServerSetAimTarget(NewTarget);
    }
}

void ADR_FlashDrone::ServerSetAimTarget_Implementation(const FVector_NetQuantize& NewTarget)
{
    CurrentAimTarget = NewTarget;
}

void ADR_FlashDrone::OnRep_AimTarget() {}

void ADR_FlashDrone::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADR_FlashDrone, CurrentAimTarget);
}

void ADR_FlashDrone::ToggleFlash(bool bOn)
{
    if (HasAuthority())
    {
        bFlashOn = bOn;
        SetLightActive(bFlashOn);
    }
    else
    {
        ServerToggleFlash(bOn);      // 클라 → 서버
    }
}

void ADR_FlashDrone::ServerToggleFlash_Implementation(bool bOn)
{
    bFlashOn = bOn;
    SetLightActive(bFlashOn);        // 서버에서 즉시 적용(→Rep)
}

void ADR_FlashDrone::OnRep_FlashOn()
{
    SetLightActive(bFlashOn);        // 원격 클라이언트 동기화
}