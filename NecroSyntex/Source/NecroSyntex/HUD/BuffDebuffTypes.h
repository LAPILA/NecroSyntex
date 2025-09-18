// BuffDebuffTypes.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BuffDebuffTypes.generated.h"

// ������ ���̺��� �� ��(Row)�� �����ϴ� ����ü
USTRUCT(BlueprintType)
struct FBuffData : public FTableRowBase
{
	GENERATED_BODY()

	// UI�� ǥ�õ� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	// ���� �� ����/������� �⺻ ���ӽð� (������ ���̺��� ����) ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 10.f;

	// �������� ��������� (UI ���Ŀ� ���)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsBuff = true;
};

// ���� ���� ���� ������ ���¸� �����ϰ� Ŭ���̾�Ʈ�� �����ϱ� ���� ����ü
USTRUCT(BlueprintType)
struct FActiveBuff
{
	GENERATED_BODY()

	// ������ ���̺��� ���� ������ ã�� ���� ID (Row Name)
	UPROPERTY()
	FName BuffID;

	// ������ ���۵� ���� �ð�
	UPROPERTY()
	float StartTime = 0.f;

	// �� ������ �� ���ӽð� (UI ProgressBar ����)
	UPROPERTY()
	float Duration = 0.f;
};