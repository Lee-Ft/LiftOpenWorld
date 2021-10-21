// Fill out your copyright notice in the Description page of Project Settings.
#include "DayNightActor.h"
#include "Engine.h"


// Sets default values
ADayNightActor::ADayNightActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsNight = false;
	CloudChangeRate = 0.1f;
	bIsTimeAuto = true;
}

// Called when the game starts or when spawned
void ADayNightActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ADayNightActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetSunPosition(DeltaTime);
	SetCloudOpacity(DeltaTime);

}


void ADayNightActor::SetSunPosition(float DeltaTime)
{
	if (LightSource)
	{
		if (bIsTimeAuto)
		{
			CurrentTime = fmod(CurrentTime + DeltaTime * TurnRate, 24);
		}
		float CurrentAngle = CurrentTime / 24.0 * 360 + 90;
		LightSource->SetActorRotation(FRotator(CurrentAngle, 0,0));
		//LightSource->AddActorLocalRotation(FRotator(DeltaTime * TurnRate, 0,0));
	}

	if (Sun)
	{
		FOutputDeviceNull ar;
		Sun->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), ar, NULL, true);
	}
}

void ADayNightActor::SetCloudOpacity(float DeltaTime)
{
	if (Sun)
	{

		FProperty* Prop = Sun->GetClass()->FindPropertyByName("Cloud opacity");
		FFloatProperty* Value = Cast<FFloatProperty>(Prop);
		if (Value)
		{
			void* ParamAddress = Value->ContainerPtrToValuePtr<float*>(Sun);
					
			//Value->SetPropertyValue(Param, 10.f);
			Sun->CanEditChange(Value);
			CurrentCloudOpacity = Value->GetPropertyValue(ParamAddress);
			if (CurrentCloudOpacity >= 3.0f)
			{
				bIsNight = true;
			}
			if (CurrentCloudOpacity <= 0.5f)
			{
				bIsNight = false;
			}

			if (bIsNight)
			{
				Value->SetPropertyValue(ParamAddress, CurrentCloudOpacity - DeltaTime * CloudChangeRate);
			}
			else
			{
				Value->SetPropertyValue(ParamAddress, CurrentCloudOpacity + DeltaTime * CloudChangeRate);
			}
			

		}
		FOutputDeviceNull ar;
		Sun->CallFunctionByNameWithArguments(TEXT("RefreshMaterial"), ar, NULL, true);

	}
}

void ADayNightActor::SetCloudColor(float)
{
	
}
