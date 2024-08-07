// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MainCharacter/MainCharacter.h"



void AShooterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


bool AShooterAIController::IsDead() const
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetPawn());
	if (MainCharacter)
	{
		return MainCharacter->IsDead();
	}
	return true;
}


void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();
	

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}
