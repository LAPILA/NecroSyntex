// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Monster.h"
#include "NecroSyntex/Monster/TenAxe_MonsterAI.h"
#include "NecroSyntex/Monster/BasicMonsterAI.h"
#include "EliteMonsterAI.h"
#include "GameFramework/Pawn.h"

void AAC_Monster::CallFindPlayer()
{
    APawn* ControlledPawn = GetPawn();
    
    if (!ControlledPawn) {
        return;
    }

    if (ATenAxe_MonsterAI* TenAxePawn = Cast<ATenAxe_MonsterAI>(ControlledPawn)) {
        TenAxePawn->FindPlayer();
    }

    if (AEliteMonsterAI* BossMonster = Cast<AEliteMonsterAI>(ControlledPawn)) {
        BossMonster->StartScreamTime(5.0f); //5초 뒤에 scream하도록 설정.
    }
}
