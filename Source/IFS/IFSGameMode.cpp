// Copyright Epic Games, Inc. All Rights Reserved.

#include "IFSGameMode.h"
#include "IFSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIFSGameMode::AIFSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
