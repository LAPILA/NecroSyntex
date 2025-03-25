// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_ChasePlayer.h"
#include "AIController.h"
#include "BasicMonsterAI.h"

UBT_ChasePlayer::UBT_ChasePlayer()
{
    NodeName = TEXT("Update Walk Speed");
}

EBTNodeResult::Type UBT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    // AI가 제어하는 Pawn 가져오기
    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return EBTNodeResult::Failed;

    // 몬스터 AI로 캐스팅
    ABasicMonsterAI* MonsterAI = Cast<ABasicMonsterAI>(AIPawn);
    if (!MonsterAI) return EBTNodeResult::Failed;

    // 몬스터 이동 속도 업데이트 (C++ 함수 호출)
    MonsterAI->UpdateWalkSpeed(MonsterAI->ChaseSpeed);

    // 태스크 성공 반환
    return EBTNodeResult::Succeeded;
}
