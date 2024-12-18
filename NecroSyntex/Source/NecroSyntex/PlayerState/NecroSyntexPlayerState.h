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
	//Score Update - Client
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);
private:
	//플레이어 접근 방법: 각각 Character, Controller Cast를 통한 접근 필요
	class APlayerCharacter* Character;
	class ANecroSyntexPlayerController* Controller;
};
