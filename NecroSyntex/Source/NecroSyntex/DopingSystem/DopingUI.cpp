// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingUI.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UDopingUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 위젯이 생성될 때 DopingComponent를 가져옵니다.
    UDC = GetDopingComponent();
    if (UDC)
    {
        UE_LOG(LogTemp, Log, TEXT("DopingComponent successfully initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to initialize DopingComponent"));
    }
}


UDopingComponent* UDopingUI::GetDopingComponent()
{
    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController not found"));
        return nullptr;
    }

    // 캐릭터 가져오기
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found"));
        return nullptr;
    }

    // 도핑 컴포넌트 가져오기
    UDopingComponent* DopingComponent = PlayerCharacter->FindComponentByClass<UDopingComponent>();
    if (!DopingComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("DopingComponent not found"));
        return nullptr;
    }

    return DopingComponent;
}

bool UDopingUI::GetFirstDopingAble()
{
    bool Able = true;
    
    if (!UDC->OneKeyDoping->Able) {
        Able = false;
    }

    if (UDC->OneKeyDoping->DopingItemNum <= 0) {
        Able = false;
    }

    return Able;
}

bool UDopingUI::GetSecondDopingAble()
{
    bool Able = true;

    if (!UDC->TwoKeyDoping->Able) {
        Able = false;
    }

    if (UDC->TwoKeyDoping->DopingItemNum <= 0) {
        Able = false;
    }

    return Able;
}