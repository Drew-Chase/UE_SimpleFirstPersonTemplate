/*
	First Person Movement - LFInteractive LLC. 2021-2025
	The First Person Movement plugin provides a simple first-person movement system for Unreal Engine 5.
	This code is part of the First Person Movement plugin, which is designed to be used with Unreal Engine 5.
	For more information, visit https://gaming.lfinteractive.com/first-person-movement
	This project is licensed under the Commercial Marketplace License.
*/

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "FPMPlayerController.generated.h"

UCLASS()
class TP_SIMPLEFIRSTPERSON_API AFPMPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	virtual void SetupInputComponent() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMovementContext;
	AFPMPlayerController();
};
