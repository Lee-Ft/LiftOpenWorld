// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkAnimNotifyState.h"

#include "Engine.h"
#include "FemaleKnightCharacter.h"
#include "WarriorCharacter.h"


void UWalkAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Cyan, __FUNCTION__);
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->MoveStart();
		}
	}
}

void UWalkAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Cyan, __FUNCTION__);
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		AWarriorCharacter* Player = Cast<AWarriorCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->MoveEnd();
		}
	}

}