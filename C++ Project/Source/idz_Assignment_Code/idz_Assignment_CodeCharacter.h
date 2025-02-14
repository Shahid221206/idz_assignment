// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "idz_Assignment_CodeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class Aidz_Assignment_CodeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Input Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Block Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockAction;

public:
	Aidz_Assignment_CodeCharacter();

	/** Character's movement direction (used in animation) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Direction;

	/** Character's movement speed (used in animation) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

protected:
	// Animation and combat-related variables and functions

	/** Animation montage for attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	/** Is the character currently attacking? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	/** Called when the attack montage ends */
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Plays the attack montage */
	void PlayAttackMontage();

	/** Is the character blocking? */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Abilities")
	bool bIsBlocking;

	/** Called when blocking starts */
	void StartBlocking();

	/** Called when blocking stops */
	void StopBlocking();

	/** Called when sprinting starts */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void StartSprinting();

	/** Called when sprinting stops */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void StopSprinting();

	/** Checks if the player is in the air */
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	bool IsPlayerInAir() const;

	/** Calculates the character's speed and direction */
	void CalculateSpeedAndDirection();

	/** Movement input handling */
	void Move(const FInputActionValue& Value);

	/** Look input handling */
	void Look(const FInputActionValue& Value);

	/** Checks if the player is in air or grounded */
	UFUNCTION(BlueprintCallable, Category = "Character|State")
	void CheckInAir();

	/** Whether the character is in the air */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|State")
	bool bInAir;

protected:
	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Adds input mapping context */
	virtual void BeginPlay() override;
};
