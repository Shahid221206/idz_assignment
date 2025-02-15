// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "idz_Assignment_CodeCharacter.generated.h"

// Forward declarations
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;

// Define a log category for the character
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class Aidz_Assignment_CodeCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	// Camera boom to position the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// Input mapping context for player controls
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// Input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockAction;

	// Animation montage for attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	// Track if the character is currently attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	// Track if the character is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|State", meta = (AllowPrivateAccess = "true"))
	bool bInAir;

	// Track if the character is blocking
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

public:
	Aidz_Assignment_CodeCharacter();

	// Movement and animation properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

protected:
	// Character setup
	virtual void BeginPlay() override;

	// Input setup
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Player input functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Sprinting
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void StopSprinting();

	// Blocking
	void StartBlocking();
	void StopBlocking();

	// Combat
	void PlayAttackMontage();
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Helper functions
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	bool IsPlayerInAir() const;

	void CalculateSpeedAndDirection();

	UFUNCTION(BlueprintCallable, Category = "Character|State")
	void CheckInAir();

private:
	// Helper to toggle rotation based on movement
	void UpdateRotationBehavior();

public:
	// Getters for camera components
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

