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
    // AI ��Ʈ�ѷ� ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    // AI�� �����ϴ� Pawn ��������
    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return EBTNodeResult::Failed;

    // ���� AI�� ĳ����
    ABasicMonsterAI* MonsterAI = Cast<ABasicMonsterAI>(AIPawn);
    if (!MonsterAI) return EBTNodeResult::Failed;

    // ���� �̵� �ӵ� ������Ʈ (C++ �Լ� ȣ��)
    MonsterAI->UpdateWalkSpeed(MonsterAI->ChaseSpeed);

    // �½�ũ ���� ��ȯ
    return EBTNodeResult::Succeeded;
}
