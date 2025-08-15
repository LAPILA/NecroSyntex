// VoiceComponent.cpp
#include "VoiceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundClass.h"
#include "Net/UnrealNetwork.h"

UVoiceComponent::UVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UVoiceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVoiceComponent, VoiceSet);
}

void UVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	/* 빈 Cue 는 쿨다운 0 으로 채워둠 */
	for (uint8 i = 0; i < static_cast<uint8>(EVoiceCue::Count); ++i)
	{
		const EVoiceCue Cue = static_cast<EVoiceCue>(i);
		CooldownTable.FindOrAdd(Cue) = CooldownTable.Contains(Cue)
			? CooldownTable[Cue]
			: 0.f;
	}
}

void UVoiceComponent::SetSoundClass(UAudioComponent* AudioComp, USoundClass* SoundClass)
{
	if (AudioComp && SoundClass)
	{
		AudioComp->SoundClassOverride = SoundClass;
		UE_LOG(LogTemp, Log, TEXT("Sound class set: %s"), *SoundClass->GetName());
	}
}

void UVoiceComponent::PlayVoice(EVoiceCue Cue, float Vol, float Pitch)
{
	/* 1) 권한 가드 ─ 소유 클라이언트(AutonomousProxy) 또는 서버만 실행 */
	ENetRole Role = GetOwner()->GetLocalRole();
	if (!(GetOwner()->HasAuthority() || Role == ROLE_AutonomousProxy)) return;

	if (!VoiceSet) return;

	USoundBase* Snd = nullptr;
	USoundAttenuation* Attn = nullptr;
	USoundClass* SoundClass = nullptr;
	VoiceSet->GetVoice(Cue, Snd, Attn, SoundClass);
	if (!Snd) return;

	const float MinInterval = CooldownTable[Cue];
	const double Now = GetWorld()->GetTimeSeconds();
	if (double* Last = LastPlayTime.Find(Cue))
		if ((Now - *Last) < MinInterval) return;          // 쿨다운 진행 중
	LastPlayTime.Add(Cue, Now);

	/* 2) 네트워크 전파 */
	if (GetOwner()->HasAuthority())
	{
		MulticastPlayVoice(Cue, Vol, Pitch);
	}
	else
	{
		ServerPlayVoice(Cue, Vol, Pitch);
	}
}

void UVoiceComponent::ServerPlayVoice_Implementation(EVoiceCue Cue, float V, float P)
{
	MulticastPlayVoice(Cue, V, P);
}

void UVoiceComponent::MulticastPlayVoice_Implementation(EVoiceCue Cue, float V, float P)
{
    if (!VoiceSet) return;

    USoundBase* Snd = nullptr;
    USoundAttenuation* Attn = nullptr;
    USoundClass* SoundClass = nullptr;
    VoiceSet->GetVoice(Cue, Snd, Attn, SoundClass);
    if (!Snd) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 사운드 클래스 이름 디버깅
    if (SoundClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Setting SoundClass: %s"), *SoundClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SoundClass is NULL"));
    }

    UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAttached(
        Snd,
        Owner->GetRootComponent(),
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::KeepRelativeOffset,
        true,
        V,
        P
    );

    if (AudioComp)
    {
        AudioComp->bAutoDestroy = true;

        // SoundClass 강제 설정
        if (SoundClass)
        {
            AudioComp->SoundClassOverride = SoundClass;
            AudioComp->SetVolumeMultiplier(V);  // 볼륨 강제 설정
            AudioComp->Play();  // 설정 후 재생
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SoundClass was not applied."));
        }

        UE_LOG(LogTemp, Log, TEXT("Voice attached to %s"), *Owner->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to attach voice to %s"), *Owner->GetName());
    }
}
