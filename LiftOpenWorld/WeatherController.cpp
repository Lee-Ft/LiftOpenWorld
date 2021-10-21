// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherController.h"
#include "Engine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMaterialLibrary.h"
//#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
// Sets default values
AWeatherController::AWeatherController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
	RainParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("mParticle"));
	RainParticleSystemComponent->SetupAttachment(RootComponent);
	RainParticleSystemComponent->SetRelativeLocation(FVector(-12.0f, 0.0f, 1.0f));
	RainParticleSystemComponent->bAutoActivate = true;
	static ConstructorHelpers::FObjectFinder<UParticleSystem>FoundParticleSystem(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'"));

	if (FoundParticleSystem.Succeeded())
	{
		RainParticleSystemComponent->SetTemplate(FoundParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection>FoundParameterCollection(TEXT("MaterialParameterCollection'/Game/Materials/MP_Weather.MP_Weather'"));

	if (FoundParameterCollection.Succeeded())
	{
		WeatherParameterCollection = FoundParameterCollection.Object;
	}

	RainParticleSystemComponent->bAutoActivate = true;
	RainLevel = 0.0f;
	ChangeRate = 0.002f;
	ChangeRateSign = true;
	RainLevelThrehold = 0.1f;
	bIsRaining = false;
}

// Called when the game starts or when spawned
void AWeatherController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeatherController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetChangeRate(0.002f);
	//FIntVector CurrentLocation = GetWorld()->OriginLocation;
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Cyan, FString::FromInt(CurrentLocation.X));
	SetRainLevel();
	if (RainLevel > 1)
	{
		ChangeRateSign = false;
	}
	if (RainLevel < 0)
	{
		ChangeRateSign = true;
	}

	ToggleRain(RainLevel);

	SetWeatherParameter();

}

void AWeatherController::SetRainLevel()
{
	/*if (RainLevel >= ChangeRate || RainLevel <= 1 - ChangeRate) {
		RainLevel += ChangeRate;
	}*/
	if (ChangeRateSign)
	{
		
		RainLevel += ChangeRate;
	}
	else
	{
		RainLevel -= ChangeRate;
	}
}

float AWeatherController::GetRainLevel()
{
	return RainLevel;
}

void AWeatherController::SetChangeRate(float changeRate)
{
	ChangeRate = changeRate;
}

float AWeatherController::GetChangeRate()
{
	return ChangeRate;
}


void AWeatherController::ToggleRain(float value)
{
	if (value > RainLevelThrehold && bIsRaining)
	{
		RainParticleSystemComponent->Activate(false);
	}
	else
	{
		RainParticleSystemComponent->Deactivate();
	}
	
}

void AWeatherController::SetWeatherParameter()
{
	
	UKismetMaterialLibrary::SetScalarParameterValue(this, WeatherParameterCollection, FName(TEXT("RainLevel")), RainLevel / 10.0f);
	float value = UKismetMaterialLibrary::GetScalarParameterValue(this, WeatherParameterCollection, FName(TEXT("RainLevel")));

}
