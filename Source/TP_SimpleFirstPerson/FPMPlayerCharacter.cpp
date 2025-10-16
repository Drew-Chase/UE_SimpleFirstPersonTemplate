/*
	First Person Movement - LFInteractive LLC. 2021-2025
	The First Person Movement plugin provides a simple first-person movement system for Unreal Engine 5.
	This code is part of the First Person Movement plugin, which is designed to be used with Unreal Engine 5.
	For more information, visit https://gaming.lfinteractive.com/first-person-movement
	This project is licensed under the Commercial Marketplace License.
*/

#include "FPMPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "VectorTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFPMPlayerCharacter::AFPMPlayerCharacter()
{
	if (this->GetCharacterMovement())
		this->GetCharacterMovement()->MaxWalkSpeed = 300.0f; // Set the default walk speed
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the CameraComponent
	this->CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->CameraComponent->SetupAttachment(RootComponent);
	this->CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // Position the camera above the capsule
	this->CameraComponent->bUsePawnControlRotation = true; // Allow the camera to rotate with the player

	// Assign the actions
	this->JumpAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Movement/IA_Jump.IA_Jump")));
	this->MoveAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Movement/IA_Move.IA_Move")));
	this->LookAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Movement/IA_Look.IA_Look")));
	this->SprintAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Movement/IA_Sprint.IA_Sprint")));
	this->CrouchAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Input/Movement/IA_Crouch.IA_Crouch")));
}


// Called to bind functionality to input
void AFPMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(this->InputComponent))
	{
		// Bind the Jump action
		if (JumpAction)
			Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPMPlayerCharacter::Jump);
		if (MoveAction)
			Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPMPlayerCharacter::HandleMoveAction);
		if (LookAction)
			Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPMPlayerCharacter::HandleLookAction);
		if (CrouchAction)
		{
			Input->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFPMPlayerCharacter::StartCrouch);
			Input->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFPMPlayerCharacter::StopCrouch);
		}
		if (SprintAction)
		{
			Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AFPMPlayerCharacter::StartSprinting);
			Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFPMPlayerCharacter::StopSprinting);
		}
	}
}

void AFPMPlayerCharacter::HandleMoveAction(const FInputActionInstance& Instance)
{
	if (const FVector2D MoveInput = Instance.GetValue().Get<FVector2D>(); Controller && (MoveInput.X != 0.0f || MoveInput.Y != 0.0f))
	{
		// Calculate the forward and right vectors based on the camera's rotation
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FVector Forward = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X);
		const FVector Right = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

		// Calculate the movement direction
		const FVector Direction = (Forward * MoveInput.Y + Right * MoveInput.X).GetSafeNormal();

		// Add movement input
		AddMovementInput(Direction);
	}
}

void AFPMPlayerCharacter::HandleLookAction(const FInputActionInstance& Instance)
{
	if (const FVector2D LookInput = Instance.GetValue().Get<FVector2D>(); Controller && (LookInput.X != 0.0f || LookInput.Y != 0.0f))
	{
		AddControllerYawInput(LookInput.X * HorizontalLookSensitivity);
		AddControllerPitchInput(LookInput.Y * VerticalLookSensitivity);
	}
}

void AFPMPlayerCharacter::StartSprinting()
{
	if (IsCrouching)return; // Prevent sprinting while crouching
	if (this->GetCharacterMovement())
		this->GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
	this->IsSprinting = true;
}

void AFPMPlayerCharacter::StopSprinting()
{
	if (IsCrouching)return; // Prevent sprinting while crouching
	if (this->GetCharacterMovement())
		this->GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier;
	this->IsSprinting = false;
}

void AFPMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if player wants to uncrouch but couldn't due to obstacle
	if (bWantsToUncrouch && IsCrouching && CanUncrouch())
	{
		StopCrouch();
	}

	if (bIsCameraAnimating)
	{
		UCapsuleComponent* Capsule = this->GetCapsuleComponent();

		// Animate capsule height
		const float CurrentHeight = Capsule->GetUnscaledCapsuleHalfHeight();
		const float NewHeight = FMath::FInterpConstantTo(CurrentHeight, CapsuleTargetHeight, DeltaTime, 100.0f); // 100 units per second
		Capsule->SetCapsuleHalfHeight(NewHeight);

		// Animate camera position
		const FVector CurrentLocation = CameraComponent->GetRelativeLocation();
		const FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, CameraTargetLocation, DeltaTime, 200.0f); // 200 units per second
		CameraComponent->SetRelativeLocation(NewLocation);

		// Check if we're close enough to the target to stop animating
		if (FMath::Abs(NewHeight - CapsuleTargetHeight) < 0.01f && FVector::Dist(NewLocation, CameraTargetLocation) < 0.01f)
		{
			Capsule->SetCapsuleHalfHeight(CapsuleTargetHeight);
			CameraComponent->SetRelativeLocation(CameraTargetLocation);
			bIsCameraAnimating = false;
		}
	}
}

void AFPMPlayerCharacter::StartCrouch()
{
	if (!IsCrouching && this->GetCharacterMovement())
	{
		this->IsCrouching = true;
		bWantsToUncrouch = false;
		this->GetCharacterMovement()->MaxWalkSpeed *= CrouchSpeedMultiplier;

		UCapsuleComponent* Capsule = this->GetCapsuleComponent();

		// Set up capsule height animation
		CapsuleStartHeight = Capsule->GetUnscaledCapsuleHalfHeight();
		CapsuleTargetHeight = CapsuleStartHeight * CrouchHeightPercent;

		// Set up camera animation
		CameraStartLocation = this->CameraComponent->GetRelativeLocation();
		CameraTargetLocation = FVector(0.0f, 0.0f, CapsuleTargetHeight);
		bIsCameraAnimating = true;
	}
}

void AFPMPlayerCharacter::StopCrouch()
{
	if (this->IsCrouching && this->GetCharacterMovement())
	{
		// Check if there's space above to uncrouch
		if (!CanUncrouch())
		{
			bWantsToUncrouch = true;
			return;
		}

		this->IsCrouching = false;
		bWantsToUncrouch = false;
		this->GetCharacterMovement()->MaxWalkSpeed /= CrouchSpeedMultiplier;

		UCapsuleComponent* Capsule = this->GetCapsuleComponent();

		// Set up capsule height animation back to standing
		CapsuleStartHeight = Capsule->GetUnscaledCapsuleHalfHeight();
		CapsuleTargetHeight = CapsuleStartHeight / CrouchHeightPercent;

		// Set up camera animation back to standing position
		CameraStartLocation = this->CameraComponent->GetRelativeLocation();
		CameraTargetLocation = FVector(0.0f, 0.0f, CapsuleTargetHeight);
		bIsCameraAnimating = true;
	}
}

bool AFPMPlayerCharacter::CanUncrouch() const
{
	UCapsuleComponent* Capsule = this->GetCapsuleComponent();
	if (!Capsule)
		return false;

	// Calculate the height difference between crouched and standing
	const float CurrentHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float StandingHeight = CurrentHeight / CrouchHeightPercent;
	const float HeightDifference = StandingHeight - CurrentHeight;

	// Get the current location and calculate trace start/end points
	const FVector CapsuleLocation = Capsule->GetComponentLocation();
	const FVector TraceStart = CapsuleLocation + FVector(0.0f, 0.0f, CurrentHeight);
	const FVector TraceEnd = TraceStart + FVector(0.0f, 0.0f, HeightDifference + 10.0f); // Add 10 units buffer

	// Set up trace parameters
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Perform a sphere trace to check for obstacles
	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(CapsuleRadius),
		QueryParams
	);

	// Return true if nothing was hit (can uncrouch), false if something is blocking
	return !bHit;
}
