// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitNotifyState.h"
#include "WarriorCharacter.h"
void UAttackHitNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->AttackHitStart();
		}
	}
}

void UAttackHitNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->AttackHitEnd();
		}
	}

}