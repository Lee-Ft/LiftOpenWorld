// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiftOpenWorldGameMode.h"
#include "BaseAICharacter.h"
#include "FemaleKnightCharacter.h"
#include "UObject/ConstructorHelpers.h"
//#include "PlayingCharacter.h"
//#include "PlayingController.h"

ALiftOpenWorldGameMode::ALiftOpenWorldGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Warrior/BP_WarriorCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;

	}

}
