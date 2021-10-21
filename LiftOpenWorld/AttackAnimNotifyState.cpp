// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAnimNotifyState.h"
#include "WarriorCharacter.h"


void UAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->SetCombo();
		}
	}
}

void UAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->ResetCombo();
		}
	}

}