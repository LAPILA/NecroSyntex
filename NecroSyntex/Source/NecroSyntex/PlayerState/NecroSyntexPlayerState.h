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
	//�÷��̾� ���� ���: ���� Character, Controller Cast�� ���� ���� �ʿ�
	UPROPERTY()
	class APlayerCharacter* Character;
	UPROPERTY()
	class ANecroSyntexPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	//아래부터는 박태혁 편집
public:
	ANecroSyntexPlayerState();

	UPROPERTY(EditAnywhere, Replicated)
	TSubclassOf<APlayerCharacter> SelectedCharacterClass;

	UPROPERTY(EditAnywhere, Replicated)
	int32 FirstDopingCode;

	UPROPERTY(EditAnywhere, Replicated)
	int32 SecondDopingCode;

	UPROPERTY(Replicated)
	bool bHasCompletedSelection = false;

	UPROPERTY(Replicated)
	bool CopyComplete = false;


protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;

};
