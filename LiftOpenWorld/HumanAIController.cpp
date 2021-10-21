// Fill out your copyright notice in the Description page of Project Settings.
#include "HumanAIController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine.h"

void AHumanAIController::OnPossess(class APawn* InPawn) {
	Super::OnPossess(InPawn);
	Bot = Cast<ACharacter>(InPawn);
	
	HomeLocation = Bot->GetActorLocation();
	NavMesh = UNavigationSystemV1::GetCurrent(this);

}

void AHumanAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
		SearchNewPoint();
}
//这个函数很重要，最好看下底层源码
void AHumanAIController::SearchNewPoint()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (NavMesh) 
	{
		FNavLocation PointAroundCharacter;
		FNavLocation RandomPt;

		// Get a random point around actor
		bool flag = NavMesh->GetRandomReachablePointInRadius(PlayerCharacter->GetActorLocation(), 200.0f, PointAroundCharacter);
		AIMoveToLocation(PointAroundCharacter.Location, RandomPt);
	}
}

void AHumanAIController::AIMoveToLocation(FVector Origin, FNavLocation& TargetPoint)
{
	const float SearchRadius = 100.0f;

	FVector DirectionVector;
	float Distance;
	NavMesh->GetRandomReachablePointInRadius(Origin, SearchRadius, TargetPoint);
	GetDistanceAndDirection(Origin, Bot->GetActorLocation(), Distance, DirectionVector);
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, FString::SanitizeFloat(Distance));
	if (Distance < 500.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, FString(TEXT("Move Around Actor")));
	}
	else
	{
		if (Distance > 2500.f)
		{
			DirectionVector.Normalize();

			FVector NewLocation = DirectionVector * 2500.f + Bot->GetActorLocation();
			NavMesh->GetRandomReachablePointInRadius(NewLocation, SearchRadius, TargetPoint);
			GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, TargetPoint.Location.ToString());
		}

		// Simple move to target point;
		MoveToLocation(TargetPoint);
	}
}

void AHumanAIController::GetDistanceAndDirection(FVector a, FVector b, float& Distance, FVector& Dir)
{
	Dir = a - b;
	Distance = Dir.Size();
	Dir.Normalize();
}



void AHumanAIController::ReArm()
{
	//GetWorldTimerManager().ClearTimer(DeadTime);
	//Bot->ReArm();
}

void AHumanAIController::StopMove()
{
	//StopMovement();
	MoveToLocation(Bot->GetActorLocation());
}