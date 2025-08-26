// Copyright NecroSyntex. All Rights Reserved.

#include "DR_FlashDrone.h"
#include "GameFramework/Pawn.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h" // FRotationMatrix 사용을 위해 추가

// =================================================================================================
// 생성자: 드론의 기본 컴포넌트와 프로퍼티를 초기화합니다.
// =================================================================================================
ADR_FlashDrone::ADR_FlashDrone()
{
	// 매 프레임마다 위치와 각도를 부드럽게 보간해야 하므로 Tick을 활성화합니다.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// 엔진의 기본 이동 리플리케이션을 끄고, 직접 만든 커스텀 보간 로직을 사용합니다.
	// 이렇게 하면 움직임이 훨씬 부드러워 보입니다.
	SetReplicateMovement(false);

	// 충돌을 위한 기본 Sphere 컴포넌트
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->InitSphereRadius(35.f);
	CollisionComp->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // 다른 Pawn과는 충돌하지 않음
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 플레이어의 카메라를 밀지 않음

	// 드론의 외형을 담당하는 Static Mesh 컴포넌트
	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	DroneMesh->SetupAttachment(RootComponent);
	DroneMesh->SetCastShadow(false);
	DroneMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 메시 또한 카메라를 밀지 않음

	// 드론의 조명 역할을 하는 SpotLight 컴포넌트
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(DroneMesh);
	SpotLight->Intensity = 50000.f;
	SpotLight->SetRelativeLocation(FVector(0, 0, 30.f));
	SpotLight->SetRelativeRotation(FRotator::ZeroRotator); // 드론의 정면을 보도록 각도 초기화
}

// =================================================================================================
// 게임 시작 시 호출됩니다.
// =================================================================================================
void ADR_FlashDrone::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 실행: 주기적으로 드론이 주인에게서 너무 멀어졌는지 확인하는 타이머를 설정합니다.
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			TeleportCheckTimer, this,
			&ADR_FlashDrone::CheckDistanceAndTeleport,
			CheckInterval, true);
	}
}

// =================================================================================================
// 네트워크로 복제(Replicate)할 변수들을 등록합니다.
// =================================================================================================
void ADR_FlashDrone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME 매크로를 사용해 아래 변수들이 서버에서 클라이언트로 동기화되도록 설정합니다.
	DOREPLIFETIME(ADR_FlashDrone, TargetActor);       // 드론이 따라다닐 주인 액터
	DOREPLIFETIME(ADR_FlashDrone, CurrentAimTarget);  // 플레이어의 현재 조준점 위치
	DOREPLIFETIME(ADR_FlashDrone, ReplicatedLocation); // 서버가 계산한 드론의 최종 위치
	DOREPLIFETIME(ADR_FlashDrone, ReplicatedRotation); // 서버가 계산한 드론의 최종 각도
}

// =================================================================================================
// 드론의 주인(따라다닐 대상)을 외부에서 설정해주는 함수입니다.
// =================================================================================================
void ADR_FlashDrone::InitFollowing(AActor* InTarget, float InMaxDist)
{
	TargetActor = InTarget;
	MaxDistance = InMaxDist;

	// 이 함수는 서버에서 스폰 직후 즉시 호출됩니다.
	// 클라이언트로 정보가 복제되기 전에 여기서 올바른 초기 위치와 각도를 설정하면,
	// 클라이언트는 처음부터 정확한 위치를 목표로 보간을 시작하여 위치/각도 오류가 사라집니다.
	if (HasAuthority() && IsValid(TargetActor))
	{
		const APawn* OwningPawn = Cast<APawn>(TargetActor);

		// 1. 초기 각도 계산: 플레이어의 카메라 방향을 기준으로 하되, 수평을 유지합니다.
		FRotator InitialRotation = TargetActor->GetActorRotation(); // 기본값
		if (OwningPawn && OwningPawn->GetController())
		{
			InitialRotation = OwningPawn->GetController()->GetControlRotation();
			InitialRotation.Pitch = 0.f; // 드론이 위/아래로 기울지 않도록 고정
			InitialRotation.Roll = 0.f;
		}

		// 2. 초기 위치 계산: Tick 함수의 고정 위치 계산 로직을 그대로 사용합니다.
		const FRotator YawRotation(0.f, InitialRotation.Yaw, 0.f);
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		const FVector InitialLocation = TargetActor->GetActorLocation() + RightVector * PivotRightOffset + FVector(0, 0, OrbitHeight);

		// 3. 계산된 위치와 각도로 즉시 이동합니다.
		SetActorLocationAndRotation(InitialLocation, InitialRotation);
	}

	// 4. 클라이언트 보간을 위한 초기 목표 지점을 설정합니다.
	//    (이때 GetActorLocation/Rotation은 이미 위에서 계산된 최종 위치/각도입니다.)
	InterpolationTargetLocation = GetActorLocation();
	InterpolationTargetRotation = GetActorRotation();
}

// =================================================================================================
// 매 프레임 호출되는 함수로, 드론의 모든 움직임 로직을 처리합니다.
// =================================================================================================
void ADR_FlashDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(TargetActor)) return;

	// 서버는 모든 계산을 직접 수행하고, 클라이언트는 서버가 보내준 값으로 보간만 수행합니다.
	if (HasAuthority())
	{
		// --- 1. 목표 위치 계산 ---
		const APawn* OwningPawn = Cast<APawn>(TargetActor);

		// 플레이어의 몸 방향(ActorRotation) 대신 카메라 방향(ControlRotation)을 사용해야 클라이언트와 오차가 없습니다.
		FRotator ViewRotation = TargetActor->GetActorRotation();
		if (OwningPawn && OwningPawn->GetController())
		{
			ViewRotation = OwningPawn->GetController()->GetControlRotation();
		}

		// Pitch, Roll을 제외한 순수 Yaw 회전값으로 안정적인 '오른쪽 벡터'를 추출합니다.
		const FRotator YawRotation(0.f, ViewRotation.Yaw, 0.f);
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 드론이 머무를 기준 위치(Base)를 계산합니다.
		FVector Base = TargetActor->GetActorLocation() + RightVector * PivotRightOffset + FVector(0, 0, OrbitHeight);

		const bool bNowAiming = CurrentAimTarget.SizeSquared() > 1.f;
		FVector DesiredLoc;
		if (bNowAiming) // 조준 중일 때: 플레이어의 조준점을 향해 특정 오프셋 위치로 이동
		{
			const FVector AimDir = (CurrentAimTarget - Base).GetSafeNormal();
			const FVector Right = FVector::CrossProduct(FVector::UpVector, AimDir).GetSafeNormal();
			const FVector Up = FVector::CrossProduct(AimDir, Right);
			DesiredLoc = Base + AimDir * AimOffset.X + Right * AimOffset.Y + Up * AimOffset.Z;
		}
		else // 평상시: 계산된 기준(Base) 위치에 머무름
		{
			DesiredLoc = Base;
		}

		// --- 2. 서버 드론 위치 보간 ---
		// 계산된 목표 위치로 부드럽게 이동시킵니다.
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLoc, DeltaTime, FollowInterpSpeed);
		FHitResult HitResult;
		SetActorLocation(NewLocation, true, &HitResult);
		if (HitResult.bBlockingHit) { SetActorLocation(HitResult.Location); }

		// --- 3. 목표 각도 계산 ---
		FVector Dir;
		if (bNowAiming) {
			Dir = (CurrentAimTarget - GetActorLocation()).GetSafeNormal();
		}
		else {
			Dir = ViewRotation.Vector(); // 평상시에는 플레이어의 카메라 방향을 바라봄
		}
		const float TurnSpd = bNowAiming ? 20.f : 10.f;
		FRotator TargetRotation = Dir.Rotation();

		// --- 4. 서버 드론 각도 보간 ---
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpd));

		// --- 5. 클라이언트에 보낼 데이터 업데이트 ---
		ReplicatedLocation = GetActorLocation();
		ReplicatedRotation = GetActorRotation();
	}
	else // 클라이언트에서 실행되는 로직
	{
		// 서버로부터 받은 ReplicatedLocation/Rotation 값으로 부드럽게 시각적 보간만 수행합니다.
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), InterpolationTargetLocation, DeltaTime, FollowInterpSpeed));
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), InterpolationTargetRotation, DeltaTime, FollowInterpSpeed));
	}
}

// =================================================================================================
// ReplicatedLocation, ReplicatedRotation 변수가 서버로부터 업데이트될 때 클라이언트에서 호출됩니다.
// =================================================================================================
void ADR_FlashDrone::OnRep_ServerState()
{
	// 서버가 보내준 새로운 위치/각도 값을 클라이언트의 보간 목표 지점으로 설정합니다.
	InterpolationTargetLocation = ReplicatedLocation;
	InterpolationTargetRotation = ReplicatedRotation;
}

// =================================================================================================
// (클라이언트 전용) 서버에게 현재 조준점 위치를 보고하는 RPC 함수입니다.
// =================================================================================================
void ADR_FlashDrone::SetAimTarget(const FVector& NewTarget)
{
	if (!HasAuthority())
	{
		ServerSetAimTarget(NewTarget);
	}
}

void ADR_FlashDrone::ServerSetAimTarget_Implementation(const FVector_NetQuantize& NewTarget)
{
	CurrentAimTarget = NewTarget;
}

// =================================================================================================
// (서버 전용) 드론이 주인에게서 너무 멀어졌는지 확인하는 함수입니다.
// =================================================================================================
void ADR_FlashDrone::CheckDistanceAndTeleport()
{
	if (TargetActor && FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation()) > FMath::Square(MaxDistance))
	{
		ForceTeleportToTarget();
	}
}

// =================================================================================================
// (서버 전용) 드론을 주인 곁으로 강제 텔레포트시키는 로직입니다.
// =================================================================================================
void ADR_FlashDrone::ForceTeleportToTarget()
{
	if (HasAuthority() && IsValid(TargetActor))
	{
		// --- 각도 수정: 플레이어 카메라의 Yaw를 사용하고, Pitch/Roll은 0으로 하여 수평을 유지합니다. ---
		FRotator TeleportRotation = TargetActor->GetActorForwardVector().Rotation();
		if (const APawn* OwningPawn = Cast<APawn>(TargetActor))
		{
			if (AController* OwnerController = OwningPawn->GetController())
			{
				TeleportRotation = OwnerController->GetControlRotation();
				TeleportRotation.Pitch = 0.f;
				TeleportRotation.Roll = 0.f;
			}
		}

		// --- 위치 수정: Tick 함수의 고정 위치 계산 로직과 동일한 방식으로 위치를 계산합니다. ---
		const FRotator YawRotation(0.f, TeleportRotation.Yaw, 0.f);
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		const FVector TeleportLocation = TargetActor->GetActorLocation() + RightVector * PivotRightOffset + FVector(0, 0, OrbitHeight);

		// 모든 클라이언트에게 이 위치와 각도로 즉시 이동하라고 명령합니다.
		Multicast_ForceTeleport(TeleportLocation, TeleportRotation);
	}
}

// =================================================================================================
// (모든 클라이언트에서 실행) 텔레포트 명령을 받아 드론의 위치/각도를 즉시 변경합니다.
// =================================================================================================
void ADR_FlashDrone::Multicast_ForceTeleport_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation); // 액터 전체를 회전시킵니다.

	// 텔레포트 후 부드러운 움직임을 위해 클라이언트의 보간 목표 지점도 즉시 갱신합니다.
	InterpolationTargetLocation = NewLocation;
	InterpolationTargetRotation = NewRotation;
}