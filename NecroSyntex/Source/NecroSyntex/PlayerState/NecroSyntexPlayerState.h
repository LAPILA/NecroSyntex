#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NecroSyntexPlayerState.generated.h"

/*
* 각 플레이어당 점수 시스템
*/
UCLASS()
class NECROSYNTEX_API ANecroSyntexPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	/**
	* Replication notifies
	*/
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private:
	//플레이어 접근 방법: 각각 Character, Controller Cast를 통한 접근 필요
	UPROPERTY()
	class APlayerCharacter* Character;
	UPROPERTY()
	class ANecroSyntexPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
