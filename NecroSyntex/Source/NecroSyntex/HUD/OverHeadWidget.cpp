// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayer(APawn* InPawn)
{
    if (!InPawn) return;

    //RemoteRole ���� LocleRole����
    ENetRole RemoteRole = InPawn->GetRemoteRole();
    FString Role;

    // Role ������ �����ϰ� ó��
    switch (RemoteRole)
    {
    case ROLE_None:
        Role = TEXT("None");
        break;
    case ROLE_SimulatedProxy:
        Role = TEXT("Simulated Proxy");
        break;
    case ROLE_AutonomousProxy:
        Role = TEXT("Autonomous Proxy");
        break;
    case ROLE_Authority:
        Role = TEXT("Authority");
        break;
    default:
        Role = TEXT("Unknown Role");
        break;
    }
    FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
    SetDisplayText(RemoteRoleString);
}

void UOverHeadWidget::NativeDestruct()
{
    RemoveFromParent();
    Super::NativeDestruct();
}