// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SwordplayGameMode.h"
#include "SwordplayHUD.h"
#include "SwordplayCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASwordplayGameMode::ASwordplayGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASwordplayHUD::StaticClass();
}
