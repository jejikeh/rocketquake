// Copyright Epic Games, Inc. All Rights Reserved.


#include "RocketquakeGameModeBase.h"

#include "Player/RocketQuakeCharacter.h"
#include "Player/RocketquakePlayerController.h"

ARocketquakeGameModeBase::ARocketquakeGameModeBase()
{
    DefaultPawnClass = ARocketQuakeCharacter::StaticClass();
    PlayerControllerClass = ARocketquakePlayerController::StaticClass();
}
