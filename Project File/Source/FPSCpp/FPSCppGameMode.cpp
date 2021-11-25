// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCppGameMode.h"
#include "FPSCppHUD.h"
#include "FPSCppCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameStateBase.h"

AFPSCppGameMode::AFPSCppGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_Character"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSCppHUD::StaticClass();
	LevelTime = 100;
	Timer = LevelTime;
}

void AFPSCppGameMode::BeginPlay()
{
	Timer = LevelTime;
}

void AFPSCppGameMode::GameEnd()
{
	AMyGameStateBase* GS = GetGameState<AMyGameStateBase>();
	if(GS)
	{
		if(GS->Score==3)
		{
			OnVictory();
		}
	}
}

void AFPSCppGameMode::OnVictory_Implementation()
{
}



