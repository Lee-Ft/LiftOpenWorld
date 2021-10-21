// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DirectionalLight.h"
#include "Misc/OutputDeviceNull.h"
#include "GameFramework/Actor.h"
#include "DayNightActor.generated.h"

UCLASS()
class LIFTOPENWORLD_API ADayNightActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADayNightActor();


	void SetSunPosition(float);

	void SetCloudOpacity(float);

	void SetCloudColor(float);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Sky")
		AActor* Sun;

	UPROPERTY(EditAnywhere, Category = "Sky")
		float TurnRate;
	UPROPERTY(EditAnywhere, Category = "Sky")
		float CurrentTime;

	UPROPERTY(EditAnywhere, Category = "Sky")
		float CloudChangeRate;

	UPROPERTY(EditAnywhere, Category = "Sky")
		ADirectionalLight* LightSource;

	UPROPERTY(EditAnywhere, Category = "Sky")
		float CurrentCloudOpacity;

	UPROPERTY(EditAnywhere, Category = "Sky")
		bool bIsTimeAuto;
	bool bIsNight;

};