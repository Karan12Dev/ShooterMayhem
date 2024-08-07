// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Shoot.h"
#include "AIController.h"
#include "Character/MainCharacter/MainCharacter.h"


UBTTask_Shoot::UBTTask_Shoot()
{
}


EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AMainCharacter* AICharacter = Cast<AMainCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (AICharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (AICharacter->IsDead()) return EBTNodeResult::Failed;
	AICharacter->PullTrigger();

	int32 AIAmmo = AICharacter->GetAmmo();
	if (AIAmmo <= 0)
	{
		AICharacter->Reload();
	}

	return EBTNodeResult::Succeeded;
}
