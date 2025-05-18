// VoiceSet.h  (Single‑Attenuation 버전)
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundAttenuation.h"
#include "VoiceCue.h"
#include "VoiceSet.generated.h"

/* ────── 한 Cue 의 “사운드 모음” ────── */
USTRUCT(BlueprintType)
struct FVoiceLines
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Voice")
	TArray<TObjectPtr<USoundBase>> Clips;

	UPROPERTY(EditDefaultsOnly, Category = "Voice",
		meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinInterval = 0.2f;

	FORCEINLINE USoundBase* GetRandomClip() const
	{
		return Clips.Num() > 0 ? Clips[FMath::RandHelper(Clips.Num())] : nullptr;
	}
};

/* ────── “캐릭터 1 명” 보이스팩 ────── */
UCLASS(BlueprintType)
class NECROSYNTEX_API UVoiceSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/* Cue → 사운드 배열             */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = "Voice", meta = (TitleProperty = "Key"))
	TMap<EVoiceCue, FVoiceLines> VoiceTable;

	/* 전 Cue 공통 Attenuation       */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voice")
	TObjectPtr<USoundAttenuation> SharedAttenuation = nullptr;

	/* 전 Cue 공통 SoundClass */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voice")
	TObjectPtr<USoundClass> SharedSoundClass = nullptr;

	/* 사운드 + Attenuation 반환     */
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void GetVoice(EVoiceCue Cue,
		USoundBase*& OutSound,
		USoundAttenuation*& OutAttenuation,
		USoundClass*& OutSoundClass) const;

private:
	UVoiceSet();          // ← **선언만!**  (본문은 cpp)
};
