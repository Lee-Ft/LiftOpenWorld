// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "LiftOpenWorldGameMode.h"
#include "BaseAICharacter.h"
#include "HumanAIController.generated.h"

/**
 * 
 */
UCLASS()
class LIFTOPENWORLD_API AHumanAIController : public AAIController
{
	GENERATED_BODY()

public:

	void GoToLocation(FVector);
	void OnPossess(class APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;//移动停止做什么

	void SearchNewPoint();
	void ReArm();
	void StopMove();
	void AIMoveToLocation(FVector Origin, FNavLocation& TargetLocation);
	void GetDistanceAndDirection(FVector a, FVector b, float& Distance, FVector& Direction);

public:
	class ACharacter* Bot;
	FVector HomeLocation;
	FTimerHandle DeadTime;
	UNavigationSystemV1* NavMesh;
	ALiftOpenWorldGameMode* GameMode;
	
	
};
