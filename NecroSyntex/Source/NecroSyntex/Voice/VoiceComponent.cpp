// Copyright © NecroSyntex
#include "VoiceComponent.h"
#include "Kismet/GameplayStatics.h"

UVoiceComponent::UVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	/* 쿨다운값이 비어 있으면 0 으로 채워 둔다 */
	for (uint8 i = 0; i < static_cast<uint8>(EVoiceCue::Count); ++i)
	{
		const EVoiceCue Cue = static_cast<EVoiceCue>(i);
		if (!CooldownTable.Contains(Cue))
		{
			CooldownTable.Add(Cue, 0.f);
		}
	}
}

void UVoiceComponent::PlayVoice(EVoiceCue Cue, float Volume, float Pitch)
{
	if (!VoiceSet) return;

	/* ------------- 쿨다운 체크 ------------- */
	const double NowSec = GetWorld()->GetTimeSeconds();
	if (const double* LastSec = LastPlayTime.Find(Cue))
	{
		const float Cooldown = CooldownTable.FindRef(Cue);
		if (Cooldown > 0.f && (NowSec - *LastSec) < Cooldown)
		{
			return; // 아직 쿨다운 안 지남
		}
	}
	LastPlayTime.Add(Cue, NowSec);

	/* ------------- 네트워크 브로드캐스트 ------------- */
	if (GetOwner()->HasAuthority())
	{
		MulticastPlayVoice(Cue, Volume, Pitch);
	}
	else
	{
		ServerPlayVoice(Cue, Volume, Pitch);
	}
}

void UVoiceComponent::ServerPlayVoice_Implementation(EVoiceCue Cue, float V, float P)
{
	MulticastPlayVoice(Cue, V, P);
}

void UVoiceComponent::MulticastPlayVoice_Implementation(EVoiceCue Cue, float Volume, float Pitch)
{
	if (!VoiceSet) return;

	USoundBase* Sound = nullptr;
	USoundAttenuation* Atten = nullptr;
	VoiceSet->GetVoice(Cue, Sound, Atten);

	if (!Sound) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Owner->GetActorLocation(),
		Volume,
		Pitch,
		0.f,           // StartTime
		Atten);        // 2 D → nullptr, 3 D → Atten Asset
}
