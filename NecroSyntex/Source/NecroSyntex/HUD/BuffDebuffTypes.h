// BuffDebuffTypes.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BuffDebuffTypes.generated.h"

// 데이터 테이블의 한 줄(Row)을 정의하는 구조체
USTRUCT(BlueprintType)
struct FBuffData : public FTableRowBase
{
	GENERATED_BODY()

	// UI에 표시될 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	// ▼▼▼ 이 버프/디버프의 기본 지속시간 (데이터 테이블에서 관리) ▼▼▼
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 10.f;

	// 버프인지 디버프인지 (UI 정렬에 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsBuff = true;
};

// 현재 적용 중인 버프의 상태를 저장하고 클라이언트에 복제하기 위한 구조체
USTRUCT(BlueprintType)
struct FActiveBuff
{
	GENERATED_BODY()

	// 데이터 테이블에서 버프 정보를 찾기 위한 ID (Row Name)
	UPROPERTY()
	FName BuffID;

	// 버프가 시작된 서버 시간
	UPROPERTY()
	float StartTime = 0.f;

	// 이 버프의 총 지속시간 (UI ProgressBar 계산용)
	UPROPERTY()
	float Duration = 0.f;
};