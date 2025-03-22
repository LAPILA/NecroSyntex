#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

/** ---------------------------------------------------------------
 *  Box Information Struct
 *  ---------------------------------------------------------------
 *  �� HitBox(�ڽ� �ݸ���)�� ��ġ, ȸ��, ũ�⸦ �����ϴ� ����ü
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
 *  Ư�� ����(Time)�� ��ϵ� ��Ʈ�ڽ����� ���� + ĳ���� ����
 */
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;  // �� �������� ��ϵ� ����(�� ����)

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	APlayerCharacter* Character;
};

/** ---------------------------------------------------------------
 *  Hitscan Server-Side Rewind Result
 *  ---------------------------------------------------------------
 *  ���� źȯ/��Ʈ��ĵ�� ���� SSR ���
 */
USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;  // �¾Ҵ��� ����

	UPROPERTY()
	bool bHeadShot;      // ��弦 ����
};

/** ---------------------------------------------------------------
 *  Shotgun SSR Result
 *  ---------------------------------------------------------------
 *  ������ ��� ���� pellet�� ���ÿ� ���� �� �����Ƿ�,
 *  ĳ���ͺ�(Ű)�� '��弦 Ƚ��', '�ٵ� Ƚ��'�� ���
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
 *  - Server-Side Rewind(SSR)�� ���� ���(������ ����) + ���� ����
 *  - źȯ(Projectile) �� ��Ʈ��ĵ, ���� �� �پ��� ���⿡ ���� SSR ����
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
	friend class APlayerCharacter; // APlayerCharacter�� private �ʵ忡 ���� ����

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	/**
	 *  Debug Function
	 *  - Ư�� FramePackage�� ��Ʈ�ڽ� ��ġ�� ����� �ڽ��� ������
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
	 *  - SSR ������ HitBox���� �̵�/����/ĳ��
	 */
	void CacheBoxPositions(APlayerCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(APlayerCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(APlayerCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	/**
	 *  Hitscan SSR
	 *  - ��Ʈ��ĵ ����(������, ���� ��)
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
	  *  ��Ʈ��ĵ ������ SSR
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
	 *  - ����ü ���⸦ ���� SSR (�ʿ� �� ���)
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
	 *  ���� �����ӵ��� �����ϴ� ���� ���� ����Ʈ
	 *  - ���� �ֱ� �������� Head, ���� ������ �������� Tail
	 */
	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;  // �ִ� 4�ʰ��� ������ ����
};
