// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "Misc/OutputDeviceNull.h"
#include "Misc/OutputDeviceDebug.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeatherController.generated.h"


UCLASS()
class LIFTOPENWORLD_API AWeatherController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeatherController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	void SetRainLevel();
	float GetRainLevel();

	void SetChangeRate(float changeRate);
	float GetChangeRate();

	void ToggleRain(float RainLevel);

	void SetWeatherParameter();


	//void SetIsRaining(bool);


public:
	float RainLevel;
	float ChangeRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* RainParticleSystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Material, meta = (AllowPrivateAccess = "true"))
		class UMaterialParameterCollection* WeatherParameterCollection;

	UPROPERTY(EditAnywhere, Category = "Rain")
		bool bIsRaining;

	UPROPERTY(EditAnywhere, Category = "Sky")
		AActor* Sun;
	/*TODO: add rain sound */

	float CloudOpacity;
private:
	bool ChangeRateSign;
	float RainLevelThrehold;

};
