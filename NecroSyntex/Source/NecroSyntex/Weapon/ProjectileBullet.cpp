// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NecroSyntex/Character/PlayerCharacter.h"


void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("총알 충돌!"));
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	//PaHu Changed End
	auto PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	TDamage = PlayerCharacter->TotalDamage;
	UE_LOG(LogTemp, Warning, TEXT("%f 데미지 총합"), TDamage);
	//Pahu Changed End
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, TDamage, OwnerController, this, UDamageType::StaticClass());
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}