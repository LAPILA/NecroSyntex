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

	//---- API --------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void PlayVoice(EVoiceCue Cue, float Volume = 1.f, float Pitch = 1.f);

	/** BP 에서 지정하는 보이스팩 (Replicated) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Voice")
	TObjectPtr<UVoiceSet> VoiceSet = nullptr;

	/** Cue 별 최소 재생 간격(초) – 안 넣으면 0 */
	UPROPERTY(EditAnywhere, Category = "Voice")
	TMap<EVoiceCue, float> CooldownTable;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/* 마지막 재생 시각 */
	TMap<EVoiceCue, double> LastPlayTime;

	/* ------------ RPC ------------ */
	UFUNCTION(Server, Reliable) void ServerPlayVoice(EVoiceCue Cue, float V, float P);
	UFUNCTION(NetMulticast, Reliable) void MulticastPlayVoice(EVoiceCue Cue, float V, float P);
};
