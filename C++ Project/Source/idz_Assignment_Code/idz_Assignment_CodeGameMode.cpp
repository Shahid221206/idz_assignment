// Copyright Epic Games, Inc. All Rights Reserved.

#include "idz_Assignment_CodeGameMode.h"
#include "idz_Assignment_CodeCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aidz_Assignment_CodeGameMode::Aidz_Assignment_CodeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
