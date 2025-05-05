// Copyright © NecroSyntex
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoiceCue.h"
#include "VoiceSet.h"
#include "VoiceComponent.generated.h"

UCLASS(ClassGroup = (Audio), meta = (BlueprintSpawnableComponent))
class NECROSYNTEX_API UVoiceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoiceComponent();

	/** 서버 권한에서 전파 ‑ Volume/Pitch 는 클라이언트에도 그대로 전달 */
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void PlayVoice(EVoiceCue Cue,
		float Volume = 1.f,
		float Pitch = 1.f);

	/** 이 캐릭터가 사용할 보이스팩 (BP 에서 지정) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	TObjectPtr<UVoiceSet> VoiceSet = nullptr;

	/** Cue 별 최소 재생 간격(초) – 0 이면 무제한 */
	UPROPERTY(EditAnywhere, Category = "Voice")
	TMap<EVoiceCue, float> CooldownTable;

protected:
	virtual void BeginPlay() override;

private:
	/* ------------- RPC ------------- */
	UFUNCTION(Server, Reliable) void ServerPlayVoice(EVoiceCue Cue, float V, float P);
	UFUNCTION(NetMulticast, Reliable) void MulticastPlayVoice(EVoiceCue Cue, float V, float P);

	/* 마지막 재생 시각 기록용 */
	TMap<EVoiceCue, double> LastPlayTime;
};
