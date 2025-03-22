#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "NecroSyntex/NecroSyntaxComponents/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
    // 1) 기본 Fire 처리 (애니메이션 재생 등)
    AWeapon::Fire(FVector());

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();
    if (InstigatorController == nullptr) return;

    // 2) MuzzleFlash 소켓에서 트레이스 시작점 계산
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket == nullptr) return;
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation();

    // 3) 먼저, 클라이언트/서버 구분 없이 “누구를 몇 번 맞췄는지”를 로컬에서 계산
    TMap<ACharacter*, uint32> HitMap;
    for (const FVector_NetQuantize& HitTarget : HitTargets)
    {
        FHitResult FireHit;
        WeaponTraceHit(TraceStart, HitTarget, FireHit);

        ACharacter* HitCharacter = Cast<ACharacter>(FireHit.GetActor());
        if (HitCharacter)
        {
            // 이미 존재하는 키라면 +=1, 처음이면 1
            if (HitMap.Contains(HitCharacter))
            {
                HitMap[HitCharacter]++;
            }
            else
            {
                HitMap.Emplace(HitCharacter, 1);
            }
        }

        // 이펙트/사운드
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                ImpactParticles,
                FireHit.ImpactPoint,
                FireHit.ImpactNormal.Rotation()
            );
        }
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound,
                FireHit.ImpactPoint,
                0.5f,
                FMath::FRandRange(-0.5f, 0.5f)
            );
        }
    }

    // 4) TArray<APlayerCharacter*>로 변환 (SSR에서 누구를 맞췄는지 전달하기 위함)
    TArray<APlayerCharacter*> HitCharacters;
    for (auto& HitPair : HitMap)
    {
        ACharacter* HitChar = HitPair.Key;
        if (!HitChar) continue;

        APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(HitChar);
        if (PlayerChar)
        {
            HitCharacters.Add(PlayerChar);
        }
    }

    /**
     * 5) 데미지 처리는 “서버 + SSR OFF”와 “클라이언트 + SSR ON”으로 분기
     *    (서버 + SSR OFF) → 서버에서 즉시 데미지
     *    (클라이언트 + SSR ON) → 서버에 RPC로 ‘ShotgunServerScoreRequest’ 요청
     */

     // (A) 서버이고 SSR을 안 쓴다면 => 즉시 데미지
    if (HasAuthority() && !bUseServerSideRewind)
    {
        for (auto& HitPair : HitMap)
        {
            ACharacter* HitChar = HitPair.Key;
            if (!HitChar) continue;

            UGameplayStatics::ApplyDamage(
                HitChar,
                Damage * HitPair.Value, // 펠릿 맞은 횟수만큼 누적
                InstigatorController,
                this,
                UDamageType::StaticClass()
            );
        }
    }
    // (B) 클라이언트이고 SSR을 사용한다면 => 서버에 ‘ShotgunServerScoreRequest’ 요청
    else if (!HasAuthority() && bUseServerSideRewind)
    {
        // 내 Pawn, Controller 캐싱
        PlayerOwnerCharacter = (PlayerOwnerCharacter == nullptr) ? Cast<APlayerCharacter>(OwnerPawn) : PlayerOwnerCharacter;
        NecroSyntexPlayerOwnerController = (NecroSyntexPlayerOwnerController == nullptr) ? Cast<ANecroSyntexPlayerController>(InstigatorController) : NecroSyntexPlayerOwnerController;

        // 로컬 컨트롤러 + LagCompensation이 존재해야 SSR 요청 가능
        if (NecroSyntexPlayerOwnerController &&
            PlayerOwnerCharacter &&
            PlayerOwnerCharacter->GetLagCompensation() &&
            PlayerOwnerCharacter->IsLocallyControlled())
        {
            PlayerOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
                HitCharacters,             // 맞춘 캐릭터들
                TraceStart,                // 시작점
                HitTargets,                // 각 펠릿 도달점
                NecroSyntexPlayerOwnerController->GetServerTime() - NecroSyntexPlayerOwnerController->SingleTripTime
            );
        }
    }

}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket == nullptr) return;

    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation();

    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

    // 펠릿을 여러 개 산포
    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        const FVector EndLoc = SphereCenter + RandVec;
        FVector ToEndLoc = EndLoc - TraceStart;
        // Distance를 그대로 늘려서 실제 트레이스 끝 점 구함
        ToEndLoc = TraceStart + ToEndLoc * (TRACE_LENGTH / ToEndLoc.Size());

        HitTargets.Add(ToEndLoc);
    }
}
