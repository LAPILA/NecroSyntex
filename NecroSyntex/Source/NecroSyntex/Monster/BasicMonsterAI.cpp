// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMonsterAI.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NecroSyntex/PlayerController/NecroSyntexPlayerController.h"
#include "AIController.h"
#include "NecroSyntex/Mission/DefenseTarget.h"
#include "NecroSyntex/NecroSyntexGameState.h"
#include "NecroSyntex/Monster/MonsterAnimInstance.h"
#include "NecroSyntex/Character/PlayerCharacter.h"
#include "M_Spawner.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABasicMonsterAI::ABasicMonsterAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkillAttackArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SkillArea"));
	SkillAttackArea->SetupAttachment(RootComponent);
	SkillAttackArea->SetRelativeLocation(FVector(168.f, 0.f, 0.f));
	SkillAttackArea->SetBoxExtent(FVector(100.f, 50.f, 50.f));
	SkillAttackArea->SetCollisionProfileName(TEXT("Trigger")); 
	SkillAttackArea->SetGenerateOverlapEvents(true);

	SkillAttackArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkillAttackArea->SetCollisionObjectType(ECC_WorldDynamic);
	SkillAttackArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkillAttackArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SkillAttackArea->SetGenerateOverlapEvents(true);

	//AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	//AttackPoint->SetupAttachment(RootComponent);
	//AttackPoint->SetCollisionProfileName(TEXT("Trigger"));
	//AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnAttackAreaOverlap);

	MonsterHP = 100.0f;
	MonsterAD = 20.0f;
	ChaseSpeed = 0.0f;
	SlowChaseSpeed = 70.0f;
	SlowTime = 3.0f;
	SkillAttackCoolTime = 15.0f;
	MonsterDistance = 50.0f;
	CanAttack = true;
	MeleeAttack = false;
	CanSkill = true;
}

// Called when the game starts or when spawned
void ABasicMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	DefaultChaseSpeed = ChaseSpeed;

	//SkillBoxComponent overlab event bind.
	if (SkillAttackArea) {
		SkillAttackArea->OnComponentBeginOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapBegin);
		SkillAttackArea->OnComponentEndOverlap.AddDynamic(this, &ABasicMonsterAI::OnSkillAreaOverlapEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SkillAttackArea is nullptr"));
	}
}

// Called every frame
void ABasicMonsterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABasicMonsterAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicMonsterAI::UpdateWalkSpeed()
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = DefaultChaseSpeed;
	}
}

//Weapon Damage
float ABasicMonsterAI::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CanAttack = false;
	// �̹� ���� ���¸� ó������ ����
	if (MonsterHP <= 0.0f) {
		return 0.0f;
	}

	// �������� ���� �÷��̾��� ��Ʈ�ѷ�
	ANecroSyntexPlayerController* DPC = Cast<ANecroSyntexPlayerController>(EventInstigator);
	APlayerCharacter* DPA = Cast<APlayerCharacter>(DPC->GetPawn());

	if (!DPA) {
		UE_LOG(LogTemp, Warning, TEXT("�ڵθ� �ٺ�"));
		return 0.0f;
	}

	if (GetCharacterMovement()) {//��ų ��� �� �������� ������ �̵��ӵ��� ������Ʈ�Ǿ� ����� �ǰ� �̷� ���� �����̵��ϴ� ����� �߻�.
		GetCharacterMovement()->MaxWalkSpeed = SlowChaseSpeed;
	}

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);

	MonsterHP -= DamageAmount + DPA->DopingDamageBuff;
	
	if (DamageAmount < 50) {//Refactoring Need..
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitAnimation();
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		PlayHitHighDamageAnimation();
	}

	// ��� ó��
	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		AController* TempController = GetController();
		AAIController* AIController = Cast<AAIController>(TempController);

		MonsterAnim->DieTime = true;

		if (AIController) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller UnPossess()"));
			AIController->UnPossess();  // AIController ����
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller Nope"));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayDeath"));

		PlayDeathAnimation();

		MonsterStopMove();

		DelayedFunction(3.5f); // ���� �ð� �� ���� �Ǵ� ������
	}
	return DamageAmount;
}

//Doping Damage
void ABasicMonsterAI::TakeDopingDamage(float DopingDamageAmount)
{
	CanAttack = false;
	//UE_LOG(LogTemp, Warning, TEXT("TakeDopingDamage1"));

	if (MonsterHP <= 0.0f) {
		return;
	}

	MonsterStopMove();

	MonsterHP -= DopingDamageAmount;//if doping take damage setting speed slowly? 
	PlayHitAnimation();

	GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, SlowTime, false);
	GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 0.02f, false);


	if (MonsterHP <= 0.0f) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
		AController* TempController = GetController();
		AAIController* AIController = Cast<AAIController>(TempController);

		MonsterAnim->DieTime = true;

		if (AIController) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller UnPossess()"));
			AIController->UnPossess();  // AIController ����
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Controller Nope"));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayDeath"));

		PlayDeathAnimation();

		MonsterStopMove();

		DelayedFunction(3.5f); // ���� �ð� �� ���� �Ǵ� ������
	}
	return;
}

void ABasicMonsterAI::MonsterStopMove()
{
	if (GetCharacterMovement()) {
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}
}

void ABasicMonsterAI::AttackCoolTime()
{
	CanAttack = true;
}

void ABasicMonsterAI::PlayHitAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
	}
}

void ABasicMonsterAI::PlayHitHighDamageAnimation()//���� �������� ��� hit �ִϸ��̼� ���
{
	if (HitReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(HitHighDamageReactionMontage);
	}
}

void ABasicMonsterAI::PlayDeathAnimation()//���� �ִϸ��̼� ���
{
	if (DeathReactionMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeathReaction Start"));
		GetMesh()->GetAnimInstance()->Montage_Play(DeathReactionMontage);
	}
}

void ABasicMonsterAI::PlaySkillAttackAnimation()//��ų ���� �ִϸ��̼� ���
{
	if (SkillAttackMontage && GetMesh() && GetMesh()->GetAnimInstance()) {
		GetMesh()->GetAnimInstance()->Montage_Play(SkillAttackMontage);
	}
}

void ABasicMonsterAI::DelayedFunction(float DelayTime)//���� �ð� ���� �񵿱������� �Լ��� ����Ǵٰ� destroy
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABasicMonsterAI::DestroyMonster, DelayTime, false);
}

void ABasicMonsterAI::DestroyMonster()
{
	Destroy();
}

void ABasicMonsterAI::Attack_Player()//c++�� ���� �õ������� ������ �̹� �صּ� �н� playx4 ���ķ� ���� ����.
{
	if (AttackMontage) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			AnimInstance->Montage_Play(AttackMontage);
			UE_LOG(LogTemp, Warning, TEXT("Playing Attack Montage"));
		}
	}
	MoveToPlayer();
}

void ABasicMonsterAI::MoveToPlayer()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ANecroSyntexGameState* GameState = Cast<ANecroSyntexGameState>(UGameplayStatics::GetGameState(GetWorld()));
	ADefenseTarget* DefenseTarget = nullptr;

	for (TObjectIterator<ADefenseTarget> It; It; ++It) {
		DefenseTarget = *It;
		if (DefenseTarget) {
			// DefenseTarget�� ã������
			break;
		}
	}

	if (Player) {
		AAIController* AIController = Cast<AAIController>(GetController());

		if (DefenseTarget && GameState) {
			if (AIController && GameState->CurrentMission == "Defense") {
				AIController->MoveToActor(DefenseTarget, 10.0f, true, true, true, 0, true);
				return;
			}
		}
		
		if (AIController && GameState->CurrentMission != "Defense") {
			if (MeleeAttack) {//���� ���� ������ �ϴ� ���.
				UE_LOG(LogTemp, Warning, TEXT("Moving to Player 0511"));
				AIController->MoveToActor(Player, MonsterDistance, true, true, true, 0, true);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Moving to Player 0511 2"));
				AIController->MoveToActor(Player, 150.0f, true, true, true, 0, true);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("move to end~~~~~~~~~~~~~"));
			}
		}
	}
}

void ABasicMonsterAI::OnSkillAreaOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//isSkillAttackTime = true;
	if (MonsterHP <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Skill"));
		return;
	}

	if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player")) {
		if (!OverlappingPlayers.Contains(OtherActor)) {
			OverlappingPlayers.Add(OtherActor);
		}
		if (CanSkill) {
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			UMonsterAnimInstance* MonsterAnim = Cast<UMonsterAnimInstance>(AnimInstance);
			MonsterAnim->isSkillAttackTime = true;

			if (MonsterAnim->isSkillAttackTime) {
				CanAttack = false;
				
				MonsterStopMove();

				UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
				//SkillAttack();
				PlaySkillAttackAnimation();

				GetWorld()->GetTimerManager().SetTimer(SpeedRestoreTimerHandle, this, &ABasicMonsterAI::UpdateWalkSpeed, 2.3f, false);
				GetWorld()->GetTimerManager().SetTimer(AttackRestoreTimerHandle, this, &ABasicMonsterAI::AttackCoolTime, 2.0f, false);
			}
		}
		else {
			return;
		}
		CanSkill = false;
		GetWorld()->GetTimerManager().SetTimer(MonsterSkillCoolTime, this, &ABasicMonsterAI::SkillCoolTime, SkillAttackCoolTime, false);
	}
}

void ABasicMonsterAI::OnSkillAreaOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappingPlayers.Contains(OtherActor)) {
		OverlappingPlayers.Remove(OtherActor);
	}
}

void ABasicMonsterAI::SkillAttack()
{
	PlaySkillAttackAnimation();
}

void ABasicMonsterAI::SkillCoolTime()
{
	CanSkill = true;
}

void ABasicMonsterAI::SpawnNiagaraEffect(FVector SpawnLocation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Spawn Niagara"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GasBombEffect, SpawnLocation);
}