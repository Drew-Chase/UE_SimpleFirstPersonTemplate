/*
	First Person Movement - LFInteractive LLC. 2021-2025
	The First Person Movement plugin provides a simple first-person movement system for Unreal Engine 5.
	This code is part of the First Person Movement plugin, which is designed to be used with Unreal Engine 5.
	For more information, visit https://gaming.lfinteractive.com/first-person-movement
	This project is licensed under the Commercial Marketplace License.
*/


#include "FPMPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

AFPMPlayerController::AFPMPlayerController()
{
	// Add default mapping contexts
	const wchar_t* const InputContextResourceLocation = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_FPMMovement.IMC_FPMMovement'");
	DefaultMovementContext = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, InputContextResourceLocation));
	DefaultMappingContexts.Add(DefaultMovementContext);
}

void AFPMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (int i = 0; i < DefaultMappingContexts.Num(); i++)
		{
			const UInputMappingContext* CurrentContext = DefaultMappingContexts[i];
			Subsystem->AddMappingContext(CurrentContext, i);
		}
	}
}

