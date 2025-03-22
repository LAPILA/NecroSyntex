#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

/** ---------------------------------------------------------------
 *  Box Information Struct
 *  ---------------------------------------------------------------
 *  각 HitBox(박스 콜리전)의 위치, 회전, 크기를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

/** ---------------------------------------------------------------
 *  Frame Package Struct
 *  ---------------------------------------------------------------
 *  특정 시점(Time)에 기록된 히트박스들의 정보 + 캐릭터 참조
 */
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;  // 이 프레임이 기록된 시점(초 단위)

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	APlayerCharacter* Character;
};

/** ---------------------------------------------------------------
 *  Hitscan Server-Side Rewind Result
 *  ---------------------------------------------------------------
 *  단일 탄환/히트스캔에 대한 SSR 결과
 */
USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;  // 맞았는지 여부

	UPROPERTY()
	bool bHeadShot;      // 헤드샷 여부
};

/** ---------------------------------------------------------------
 *  Shotgun SSR Result
 *  ---------------------------------------------------------------
 *  샷건의 경우 여러 pellet이 동시에 맞을 수 있으므로,
 *  캐릭터별(키)로 '헤드샷 횟수', '바디샷 횟수'를 기록
 */
USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<APlayerCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<APlayerCharacter*, uint32> BodyShots;
};

/**
 *  ---------------------------------------------------------------
 *  LagCompensationComponent
 *  ---------------------------------------------------------------
 *  - Server-Side Rewind(SSR)을 위한 기록(프레임 저장) + 판정 로직
 *  - 탄환(Projectile) 및 히트스캔, 샷건 등 다양한 무기에 대한 SSR 지원
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NECROSYNTEX_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** -------------------------------
	 *  Constructor / Basic Overrides
	 *  -------------------------------
	 */
	ULagCompensationComponent();
	friend class APlayerCharacter; // APlayerCharacter가 private 필드에 접근 가능

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	/**
	 *  Debug Function
	 *  - 특정 FramePackage의 히트박스 위치를 디버그 박스로 보여줌
	 */
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

protected:
	/** -------------------------------
	 *  Lifecycle
	 *  -------------------------------
	 */
	virtual void BeginPlay() override;

	/** -------------------------------
	 *  Core Saving / Frame Logic
	 *  -------------------------------
	 */
	void SaveFramePackage(FFramePackage& Package);
	void SaveFramePackage();
	FFramePackage InterpBetweenFrames(
		const FFramePackage& OlderFrame,
		const FFramePackage& YoungerFrame,
		float HitTime
	);
	FFramePackage GetFrameToCheck(APlayerCharacter* HitCharacter, float HitTime);

	/**
	 *  HitBox Helpers
	 *  - SSR 시점에 HitBox들을 이동/복원/캐싱
	 */
	void CacheBoxPositions(APlayerCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(APlayerCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	/**
	 *  Hitscan SSR
	 *  - 히트스캔 로직(라이플, 권총 등)
	 */
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation
	);

public:
	/** -------------------------------
	 *  Public SSR Interfaces
	 *  -------------------------------
	 */
	 /**
	  *  히트스캔 무기의 SSR
	  */
	FServerSideRewindResult ServerSideRewind(
		class APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
	);

	/**
	 *  Shotgun SSR
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<APlayerCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

	/**
	 *  Projectile SSR
	 *  - 투사체 무기를 위한 SSR (필요 시 사용)
	 */
	FServerSideRewindResult ProjectileServerSideRewind(
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/** -------------------------------
	 *  Server RPCs
	 *  -------------------------------
	 */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class AWeapon* DamageCauser
	);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<APlayerCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		APlayerCharacter *HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
protected:
	/**
	 *  Projectile SSR Internal Confirm
	 */
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/**
	 *  Shotgun SSR Internal Confirm
	 */
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);

private:
	/** -------------------------------
	 *  Internal Data
	 *  -------------------------------
	 */
	UPROPERTY()
	APlayerCharacter* Character;

	UPROPERTY()
	class ANecroSyntexPlayerController* Controller;

	/**
	 *  과거 프레임들을 저장하는 이중 연결 리스트
	 *  - 가장 최근 프레임이 Head, 가장 오래된 프레임이 Tail
	 */
	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;  // 최대 4초간의 프레임 저장
};
