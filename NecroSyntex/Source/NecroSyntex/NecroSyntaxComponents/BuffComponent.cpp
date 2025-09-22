#include "BuffComponent.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "NecroSyntex/HUD/NecroSyntexHud.h"
#include "NecroSyntex/HUD/CharacterOverlay.h"
#include "NecroSyntex/HUD/BuffDebuffContainerWidget.h"
#include "Net/UnrealNetwork.h"

UBuffComponent::UBuffComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UBuffComponent::BeginPlay()
{
    Super::BeginPlay();
    CharacterOwner = Cast<APlayerCharacter>(GetOwner());
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBuffComponent, ActiveBuffs);
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner()->HasAuthority()) return;

    bool bChanged = false;
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
    {
        // ▼▼▼ 수정된 만료 체크 로직 ▼▼▼
        if (ActiveBuffs[i].Duration > 0 && CurrentTime > ActiveBuffs[i].StartTime + ActiveBuffs[i].Duration)
        {
            ActiveBuffs.RemoveAt(i);
            bChanged = true;
        }
    }
    if (bChanged)
    {
        OnRep_ActiveBuffs();
    }
}

void UBuffComponent::AddBuff(FName BuffID, float Duration)
{
    Server_AddBuff(BuffID, Duration);
}

void UBuffComponent::RemoveBuff(FName BuffID)
{
    Server_RemoveBuff(BuffID);
}

void UBuffComponent::Server_AddBuff_Implementation(FName BuffID, float Duration)
{
    if (!BuffDataTable || Duration <= 0.f) return;

    const FBuffData* BuffData = BuffDataTable->FindRow<FBuffData>(BuffID, "");
    if (!BuffData) return;

    FActiveBuff* ExistingBuff = ActiveBuffs.FindByPredicate([&](const FActiveBuff& Buff) { return Buff.BuffID == BuffID; });
    if (ExistingBuff)
    {
        ExistingBuff->StartTime = GetWorld()->GetTimeSeconds();
        ExistingBuff->Duration = Duration;
    }
    else
    {
        FActiveBuff NewBuff;
        NewBuff.BuffID = BuffID;
        NewBuff.StartTime = GetWorld()->GetTimeSeconds();
        NewBuff.Duration = Duration;
        ActiveBuffs.Add(NewBuff);
    }
    OnRep_ActiveBuffs();
}

void UBuffComponent::Server_RemoveBuff_Implementation(FName BuffID)
{
    bool bChanged = false;
    for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
    {
        if (ActiveBuffs[i].BuffID == BuffID)
        {
            ActiveBuffs.RemoveAt(i);
            bChanged = true;
            break;
        }
    }
    if (bChanged)
    {
        OnRep_ActiveBuffs();
    }
}

void UBuffComponent::OnRep_ActiveBuffs()
{
    UpdateUI();
}

void UBuffComponent::UpdateUI()
{
    if (!CharacterOwner || !CharacterOwner->IsLocallyControlled()) return;

    ANecroSyntexPlayerController* PC = Cast<ANecroSyntexPlayerController>(CharacterOwner->GetController());
    if (PC)
    {
        ANecroSyntexHud* HUD = PC->GetHUD<ANecroSyntexHud>();
        if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->BuffDebuffContainer && BuffDataTable)
        {
            HUD->CharacterOverlay->BuffDebuffContainer->UpdateBuffs(ActiveBuffs, BuffDataTable);
        }
    }
}