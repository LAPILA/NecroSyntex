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
	ULagCompensationComponent();
	friend class APlayerCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	/**
	* Hitscan
	*/
	FServerSideRewindResult ServerSideRewind(
		class APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime);

	/**
	* Projectile
	*/
	FServerSideRewindResult ProjectileServerSideRewind(
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/**
	* Shotgun
	*/
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<APlayerCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<APlayerCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	void CacheBoxPositions(APlayerCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(APlayerCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(APlayerCharacter* HitCharacter, float HitTime);

	/**
	* Hitscan
	*/
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);

	/**
	* Projectile
	*/
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		APlayerCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/**
	* Shotgun
	*/

	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);

private:

	UPROPERTY()
	APlayerCharacter* Character;

	UPROPERTY()
	class ANecroSyntexPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

public:


};
