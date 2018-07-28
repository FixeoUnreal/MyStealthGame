// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MyStealthGameGameMode.h"
#include "MyStealthGameHUD.h"
#include "MyStealthGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyStealthGameGameMode::AMyStealthGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyStealthGameHUD::StaticClass();
}
