// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Monster.h"
#include "NecroSyntex/Monster/TenAxe_MonsterAI.h"
#include "NecroSyntex/Monster/BasicMonsterAI.h"
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
}
