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
    // 1) �⺻ Fire ó�� (�ִϸ��̼� ��� ��)
    AWeapon::Fire(FVector());

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();
    if (InstigatorController == nullptr) return;

    // 2) MuzzleFlash ���Ͽ��� Ʈ���̽� ������ ���
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket == nullptr) return;
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation();

    // 3) ����, Ŭ���̾�Ʈ/���� ���� ���� �������� �� �� ����������� ���ÿ��� ���
    TMap<ACharacter*, uint32> HitMap;
    for (const FVector_NetQuantize& HitTarget : HitTargets)
    {
        FHitResult FireHit;
        WeaponTraceHit(TraceStart, HitTarget, FireHit);

        ACharacter* HitCharacter = Cast<ACharacter>(FireHit.GetActor());
        if (HitCharacter)
        {
            // �̹� �����ϴ� Ű��� +=1, ó���̸� 1
            if (HitMap.Contains(HitCharacter))
            {
                HitMap[HitCharacter]++;
            }
            else
            {
                HitMap.Emplace(HitCharacter, 1);
            }
        }

        // ����Ʈ/����
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

    // 4) TArray<APlayerCharacter*>�� ��ȯ (SSR���� ������ ������� �����ϱ� ����)
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
     * 5) ������ ó���� ������ + SSR OFF���� ��Ŭ���̾�Ʈ + SSR ON������ �б�
     *    (���� + SSR OFF) �� �������� ��� ������
     *    (Ŭ���̾�Ʈ + SSR ON) �� ������ RPC�� ��ShotgunServerScoreRequest�� ��û
     */

     // (A) �����̰� SSR�� �� ���ٸ� => ��� ������
    if (HasAuthority() && !bUseServerSideRewind)
    {
        for (auto& HitPair : HitMap)
        {
            ACharacter* HitChar = HitPair.Key;
            if (!HitChar) continue;

            UGameplayStatics::ApplyDamage(
                HitChar,
                Damage * HitPair.Value, // �縴 ���� Ƚ����ŭ ����
                InstigatorController,
                this,
                UDamageType::StaticClass()
            );
        }
    }
    // (B) Ŭ���̾�Ʈ�̰� SSR�� ����Ѵٸ� => ������ ��ShotgunServerScoreRequest�� ��û
    else if (!HasAuthority() && bUseServerSideRewind)
    {
        // �� Pawn, Controller ĳ��
        PlayerOwnerCharacter = (PlayerOwnerCharacter == nullptr) ? Cast<APlayerCharacter>(OwnerPawn) : PlayerOwnerCharacter;
        NecroSyntexPlayerOwnerController = (NecroSyntexPlayerOwnerController == nullptr) ? Cast<ANecroSyntexPlayerController>(InstigatorController) : NecroSyntexPlayerOwnerController;

        // ���� ��Ʈ�ѷ� + LagCompensation�� �����ؾ� SSR ��û ����
        if (NecroSyntexPlayerOwnerController &&
            PlayerOwnerCharacter &&
            PlayerOwnerCharacter->GetLagCompensation() &&
            PlayerOwnerCharacter->IsLocallyControlled())
        {
            PlayerOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
                HitCharacters,             // ���� ĳ���͵�
                TraceStart,                // ������
                HitTargets,                // �� �縴 ������
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

    // �縴�� ���� �� ����
    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        const FVector EndLoc = SphereCenter + RandVec;
        FVector ToEndLoc = EndLoc - TraceStart;
        // Distance�� �״�� �÷��� ���� Ʈ���̽� �� �� ����
        ToEndLoc = TraceStart + ToEndLoc * (TRACE_LENGTH / ToEndLoc.Size());

        HitTargets.Add(ToEndLoc);
    }
}
