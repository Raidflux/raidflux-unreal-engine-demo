// Copyright Epic Games, Inc. All Rights Reserved.

#include "RaidfluxDemoGameMode.h"
#include "RaidfluxDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARaidfluxDemoGameMode::ARaidfluxDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
