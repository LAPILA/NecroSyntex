// Fill out your copyright notice in the Description page of Project Settings.

#include "DeleteMonster.h"
#include <Kismet/GameplayStatics.h>
#include "NecroSyntex/Monster/BasicMonsterAI.h"

// Sets default values
ADeleteMonster::ADeleteMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ADeleteMonster::DeleteMonster()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasicMonsterAI::StaticClass(), FoundMonsters);
	for (AActor* monster : FoundMonsters) {
		if (monster->IsA(ABasicMonsterAI::StaticClass())) {
		
			//FVector monsterLocation = monster->GetActorLocation();

			// ����� �޽����� ȭ�鿡 ��� (3�� ����)
			//FString locationText = FString::Printf(TEXT("Monster Location: X = %f, Y = %f, Z = %f"), monsterLocation.X, monsterLocation.Y, monsterLocation.Z);

			// ����� �޽����� ȭ�鿡 ǥ��
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, locationText);

			monster->Destroy();
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Cut!!!"));
		}
	}
}

// Called when the game starts or when spawned
void ADeleteMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
//void ADeleteMonster::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

