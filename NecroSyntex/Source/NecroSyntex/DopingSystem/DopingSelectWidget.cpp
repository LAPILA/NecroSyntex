// Fill out your copyright notice in the Description page of Project Settings.


#include "DopingSelectWidget.h"
#include "NecroSyntex\Character\PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UDopingSelectWidget::NativeConstruct()
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


void UDopingSelectWidget::FirstKeyDopingChoice(int32 num)
{

    UDC->SetFirstDopingKey(num);
}

void UDopingSelectWidget::SecondkeyDopingChoice(int32 num)
{

    UDC->SetSecondDopingKey(num);
}

UDopingComponent* UDopingSelectWidget::GetDopingComponent()
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

    Player = PlayerCharacter;

    // 도핑 컴포넌트 가져오기
    UDopingComponent* DopingComponent = PlayerCharacter->FindComponentByClass<UDopingComponent>();
    if (!DopingComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("DopingComponent not found"));
        return nullptr;
    }

    return DopingComponent;
}

APlayerCharacter* UDopingSelectWidget::GetDopingPlayerCharacter()
{

    return Player;
}