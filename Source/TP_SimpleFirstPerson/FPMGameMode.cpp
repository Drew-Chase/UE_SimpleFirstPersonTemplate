/*
	First Person Movement - LFInteractive LLC. 2021-2025
	The First Person Movement plugin provides a simple first-person movement system for Unreal Engine 5.
	This code is part of the First Person Movement plugin, which is designed to be used with Unreal Engine 5.
	For more information, visit https://gaming.lfinteractive.com/first-person-movement
	This project is licensed under the Commercial Marketplace License.
*/


#include "FPMGameMode.h"

#include "FPMPlayerCharacter.h"
#include "FPMPlayerController.h"

AFPMGameMode::AFPMGameMode()
{
	// Set the default pawn class to the player character class
	DefaultPawnClass = AFPMPlayerCharacter::StaticClass();

	// Set the player controller class
	PlayerControllerClass = AFPMPlayerController::StaticClass();
}
