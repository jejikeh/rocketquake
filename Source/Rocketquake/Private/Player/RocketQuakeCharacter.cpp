// Rocketquake, jejikeh


#include "Player/RocketQuakeCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/HealthComponent.h"
#include "Components/RocketquakeMovementComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"
#include "Player/RocketquakePlayerController.h"
#include "Weapon/RocketquakeWeapon.h"

ARocketQuakeCharacter::ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<URocketquakeMovementComponent>(CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
    HealthComponent->SetIsReplicated(true);
    HealthComponent->SetNetAddressable();

    TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>("TextRenderComponent");
    TextRenderComponent->SetupAttachment(GetRootComponent());
}

void ARocketQuakeCharacter::BeginPlay()
{
    Super::BeginPlay();

    BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    HealthComponent->OnDeath.AddDynamic(this, &ARocketQuakeCharacter::Multicast_OnDeath);
    HealthComponent->OnHealthChanged.AddDynamic(this, &ARocketQuakeCharacter::Client_OnHealthChanged);
    Client_OnHealthChanged();

    if (const auto PlayerController = Cast<ARocketquakePlayerController>(GetController()))
    {
        if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    SpawnWeapon();
}

void ARocketQuakeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARocketQuakeCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveForwardCharacter);
        EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveRightCharacter);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::LookCharacter);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::Jump);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::HandleStartSprintAction);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::HandleStopSprintAction);
    }
}

bool ARocketQuakeCharacter::IsSprinting() const
{
    return bIsSprinting && bIsMovingForward && !GetVelocity().IsZero();
}

float ARocketQuakeCharacter::GetMovementDirection() const
{
    const auto Velocity = GetVelocity().GetSafeNormal();
    const auto Angle = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Velocity));
    const auto Cross = FVector::CrossProduct(GetActorForwardVector(), Velocity);
    return FMath::RadiansToDegrees(Angle) * FMath::Sign(Cross.Z);
}

void ARocketQuakeCharacter::MoveForwardCharacter(const FInputActionValue &Value)
{
    const auto MovementVector = Value.Get<FVector2D>();
    Server_SetMovingForward(MovementVector.X < 0.0f);
    AddMovementInput(GetActorForwardVector(), -MovementVector.X);
}

void ARocketQuakeCharacter::MoveRightCharacter(const FInputActionValue &Value)
{
    const auto MovementVector = Value.Get<FVector2D>();
    AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void ARocketQuakeCharacter::LookCharacter(const FInputActionValue &Value)
{
    const auto LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(-LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}

void ARocketQuakeCharacter::Server_SetMovingForward_Implementation(bool IsMovingForward)
{
    bIsMovingForward = IsMovingForward;
}

void ARocketQuakeCharacter::OnRep_ToggleSprint()
{
    if (IsSprinting())
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SprintModifier;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    }
}

void ARocketQuakeCharacter::Multicast_OnDeath_Implementation()
{
    PlayAnimMontage(DeathAnimMontage);

    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(5.0f);
    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ARocketQuakeCharacter::Client_OnHealthChanged_Implementation()
{
    TextRenderComponent->SetText(FText::FromString(FString::Printf(TEXT("Health: %f"), HealthComponent->GetHealth())));
}

void ARocketQuakeCharacter::SpawnWeapon()
{
    if (!WeaponClass->IsValidLowLevelFast())
    {
        return;
    }
    
    const auto Weapon = GetWorld()->SpawnActor<ARocketquakeWeapon>(WeaponClass);
    Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
    Weapon->SetOwner(this);
}

void ARocketQuakeCharacter::HandleStartSprintAction()
{
    Server_SetSprint(true);
}

void ARocketQuakeCharacter::HandleStopSprintAction()
{
    Server_SetSprint(false);
}

void ARocketQuakeCharacter::Server_SetSprint_Implementation(bool Sprinting)
{
    bIsSprinting = Sprinting;
    OnRep_ToggleSprint();
}

// Why it doesn't work?
void ARocketQuakeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARocketQuakeCharacter, bIsSprinting);
    DOREPLIFETIME(ARocketQuakeCharacter, bIsMovingForward);
}
