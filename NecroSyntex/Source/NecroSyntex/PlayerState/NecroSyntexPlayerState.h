#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NecroSyntexPlayerState.generated.h"

/*
* �� �÷��̾�� ���� �ý���
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
	//�÷��̾� ���� ���: ���� Character, Controller Cast�� ���� ���� �ʿ�
	class APlayerCharacter* Character;
	class ANecroSyntexPlayerController* Controller;
};
