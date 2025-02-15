// Copyright Epic Games, Inc. All Rights Reserved.

#include "idz_Assignment_CodeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Aidz_Assignment_CodeCharacter

Aidz_Assignment_CodeCharacter::Aidz_Assignment_CodeCharacter()
{
    // Set up collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Configure rotation behavior
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Character movement settings
    GetCharacterMovement()->bOrientRotationToMovement = false; // Disable orient to movement
    GetCharacterMovement()->bUseControllerDesiredRotation = true; // Enable controller desired rotation
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // Set rotation rate
    GetCharacterMovement()->JumpZVelocity = 1000.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 200.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Set up camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Set up follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Initialize variables
    bIsBlocking = false;
    bIsAttacking = false;
    Speed = 0.0f;
    Direction = 0.0f;
}

void Aidz_Assignment_CodeCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void Aidz_Assignment_CodeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CalculateSpeedAndDirection();
    CheckInAir();
}

//////////////////////////////////////////////////////////////////////////
// Input Setup

void Aidz_Assignment_CodeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bind actions
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Aidz_Assignment_CodeCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Aidz_Assignment_CodeCharacter::Look);
        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &Aidz_Assignment_CodeCharacter::StartBlocking);
        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &Aidz_Assignment_CodeCharacter::StopBlocking);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &Aidz_Assignment_CodeCharacter::StartSprinting);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &Aidz_Assignment_CodeCharacter::StopSprinting);

        if (AttackAction)
        {
            EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &Aidz_Assignment_CodeCharacter::PlayAttackMontage);
        }
        else
        {
            UE_LOG(LogTemplateCharacter, Warning, TEXT("AttackAction is not assigned in the editor!"));
        }
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, update this C++ file."), *GetNameSafe(this));
    }
}

//////////////////////////////////////////////////////////////////////////
// Movement and Camera

void Aidz_Assignment_CodeCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller)
    {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void Aidz_Assignment_CodeCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

//////////////////////////////////////////////////////////////////////////
// Abilities

bool Aidz_Assignment_CodeCharacter::IsPlayerInAir() const
{
    return !GetCharacterMovement()->IsMovingOnGround();
}

void Aidz_Assignment_CodeCharacter::StartBlocking()
{
    if (IsPlayerInAir())
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("Cannot block while in the air!"));
        return;
    }

    bIsBlocking = true;
    GetCharacterMovement()->MaxWalkSpeed = 200.f; // Reduce walk speed
    UE_LOG(LogTemplateCharacter, Log, TEXT("Blocking Started"));
}

void Aidz_Assignment_CodeCharacter::StopBlocking()
{
    bIsBlocking = false;
    GetCharacterMovement()->MaxWalkSpeed = 200.f; // Reset to walk speed
    UE_LOG(LogTemplateCharacter, Log, TEXT("Blocking Stopped"));
}

void Aidz_Assignment_CodeCharacter::StartSprinting()
{
    if (!bIsBlocking) // Check if blocking is active
    {
        GetCharacterMovement()->MaxWalkSpeed = 500.f;
        UE_LOG(LogTemplateCharacter, Log, TEXT("Sprint Started"));
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot Sprint while Blocking"));
    }
}

void Aidz_Assignment_CodeCharacter::StopSprinting()
{
    if (!bIsBlocking) // Check if blocking is active
    {
        GetCharacterMovement()->MaxWalkSpeed = 200.f;
        UE_LOG(LogTemplateCharacter, Log, TEXT("Sprint Stopped"));
    }
}

void Aidz_Assignment_CodeCharacter::PlayAttackMontage()
{
    if (IsPlayerInAir())
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("Cannot attack while in the air!"));
        return;
    }

    if (bIsAttacking)
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("Attack is already in progress!"));
        return;
    }

    if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
    {
        bIsAttacking = true;
        GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
        UE_LOG(LogTemplateCharacter, Log, TEXT("Playing attack montage"));

        // Bind the OnMontageEnded event to reset the attacking state
        FOnMontageEnded Delegate;
        Delegate.BindUObject(this, &Aidz_Assignment_CodeCharacter::OnAttackMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(Delegate, AttackMontage);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("Attack montage not set or AnimInstance is missing"));
    }
}

void Aidz_Assignment_CodeCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == AttackMontage)
    {
        bIsAttacking = false;
        UE_LOG(LogTemplateCharacter, Log, TEXT("Attack Montage Finished"));
    }
}

//////////////////////////////////////////////////////////////////////////
// Utility

void Aidz_Assignment_CodeCharacter::CalculateSpeedAndDirection()
{
    if (!Controller) return;

    FVector Velocity = GetCharacterMovement()->Velocity;
    Speed = Velocity.Size();

    if (Speed > 0.f)
    {
        FRotator ActorRotation = GetActorRotation();
        FRotator MovementRotation = Velocity.ToOrientationRotator();
        Direction = FMath::FindDeltaAngleDegrees(ActorRotation.Yaw, MovementRotation.Yaw);
    }
    else
    {
        Direction = 0.f;
    }
}

void Aidz_Assignment_CodeCharacter::CheckInAir()
{
    // Check if the player is on the ground
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        bInAir = false; // Player is on the ground
    }
    else
    {
        bInAir = true; // Player is in the air
    }
}
