/*
	First Person Movement - LFInteractive LLC. 2021-2025
	The First Person Movement plugin provides a simple first-person movement system for Unreal Engine 5.
	This code is part of the First Person Movement plugin, which is designed to be used with Unreal Engine 5.
	For more information, visit https://gaming.lfinteractive.com/first-person-movement
	This project is licensed under the Commercial Marketplace License.
*/

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "FPMPlayerCharacter.generated.h"

UCLASS()
class TP_SIMPLEFIRSTPERSON_API AFPMPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsSprinting = false;
	UPROPERTY()
	bool IsCrouching = false;
	UPROPERTY()
	bool bWantsToUncrouch = false;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	// Player Options
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Player Options", meta=(AllowPrivateAccess = "true", DisplayName = "Sprint Speed Multiplier"))
	float SprintSpeedMultiplier = 2.5f;
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Player Options", meta=(AllowPrivateAccess = "true", DisplayName = "Crouch Speed Multiplier"))
	float CrouchSpeedMultiplier = 0.25f;
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Player Options", meta=(AllowPrivateAccess = "true", DisplayName = "Vertical Look Sensitivity"))
	float VerticalLookSensitivity = 1.0f;
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Player Options", meta=(AllowPrivateAccess = "true", DisplayName = "Horizontal Look Sensitivity"))
	float HorizontalLookSensitivity = 1.0f;
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Player Options", meta=(AllowPrivateAccess = "true", DisplayName = "Crouch Height Percent"))
	float CrouchHeightPercent = 0.5f;


	UPROPERTY()
	bool bIsCameraAnimating = false;
	UPROPERTY()
	FVector CameraStartLocation;
	UPROPERTY()
	FVector CameraTargetLocation;
	UPROPERTY()
	float CameraAnimationSpeed = 5.0f;

	UPROPERTY()
	float CapsuleStartHeight;
	UPROPERTY()
	float CapsuleTargetHeight;


	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true", DisplayName = "Jump Action"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true", DisplayName = "Move Action"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true", DisplayName = "Look Action"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true", DisplayName = "Crouch Action"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true", DisplayName = "Sprint Action"))
	UInputAction* SprintAction;

	// Input Action Handlers
	void HandleMoveAction(const FInputActionInstance& Instance);
	void HandleLookAction(const FInputActionInstance& Instance);

	void StartSprinting();
	void StopSprinting();

	void StartCrouch();
	void StopCrouch();
	bool CanUncrouch() const;

public:
	// Sets default values for this character's properties
	AFPMPlayerCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
};
