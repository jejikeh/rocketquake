// Rocketquake, jejikeh


#include "Player/RocketQuakeCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RocketquakeMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/RocketquakePlayerController.h"

ARocketQuakeCharacter::ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<URocketquakeMovementComponent>(CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetMesh(), "CameraSocket");
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
    HealthComponent->SetIsReplicated(true);
    HealthComponent->SetNetAddressable();

    CameraCollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CameraCollisionSphereComponent"));
    CameraCollisionSphereComponent->SetupAttachment(CameraComponent);
    CameraCollisionSphereComponent->SetSphereRadius(15.0f);
    CameraCollisionSphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>("WeaponComponent");
}

void ARocketQuakeCharacter::BeginPlay()
{
    Super::BeginPlay();

    BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    HealthComponent->OnDeath.AddDynamic(this, &ARocketQuakeCharacter::Multicast_OnDeath);
    HealthComponent->OnHealthChanged.AddDynamic(this, &ARocketQuakeCharacter::Client_OnHealthChanged);
    Client_OnHealthChanged();

    CameraCollisionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketQuakeCharacter::OnCameraCollisionBeginOverlap);
    CameraCollisionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ARocketQuakeCharacter::OnCameraCollisionEndOverlap);

    if (const auto PlayerController = Cast<ARocketquakePlayerController>(GetController()))
    {
        if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ARocketQuakeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFirstTimeTick)
    {
        Multicast_SpawnNiagaraSpawnSystemOnSpawn();
        bIsFirstTimeTick = false;
    }
}

void ARocketQuakeCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveForwardCharacter);
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this,
            &ARocketQuakeCharacter::ResetMoveForwardCharacter);

        EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveRightCharacter);

        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::LookCharacter);

        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::Jump);

        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::HandleStartSprintAction);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::HandleStopSprintAction);

        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::StartShoot);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, WeaponComponent, &UWeaponComponent::StopShoot);

        EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::NextWeapon);

        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::Reload);
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

void ARocketQuakeCharacter::SetPlayerColor_Implementation(FLinearColor NewColor)
{
    const auto MaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
    if (!MaterialInstance)
    {
        return;
    }

    MaterialInstance->SetVectorParameterValue(MaterialColorName, NewColor);
}

void ARocketQuakeCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    CheckCameraOverlap();
}

void ARocketQuakeCharacter::CheckCameraOverlap() const
{
    const auto HideMesh = CameraCollisionSphereComponent->IsOverlappingComponent(GetCapsuleComponent());
    GetMesh()->SetOwnerNoSee(HideMesh);
}

void ARocketQuakeCharacter::OnCameraCollisionEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    CheckCameraOverlap();
}

void ARocketQuakeCharacter::MoveForwardCharacter(const FInputActionValue &Value)
{
    const auto MovementVector = Value.Get<FVector2D>();
    Server_SetMovingForward(MovementVector.X < 0.0f);
    AddMovementInput(GetActorForwardVector(), -MovementVector.X);
}

void ARocketQuakeCharacter::ResetMoveForwardCharacter(const FInputActionValue &Value)
{
    Multicast_ResetMoveForwardCharacter();
}

void ARocketQuakeCharacter::Multicast_ResetMoveForwardCharacter_Implementation()
{
    if (IsLocallyControlled())
    {
        Server_SetMovingForward(false);
    }

    Server_SetMovingForward_Implementation(false);
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
    Multicast_ToggleSprint();
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
    // PlayAnimMontage(DeathAnimMontage);

    GetCapsuleComponent()->SetCollisionResponseToChannels(ECR_Ignore);
    WeaponComponent->StopShoot();

    Server_OnDeath();

    GetCharacterMovement()->DisableMovement();

    // SetLifeSpan(5.0f);
    //
    // if (Controller)
    // {
    //     Controller->ChangeState(NAME_Spectating);
    // }
    //

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

}

void ARocketQuakeCharacter::Server_OnDeath_Implementation()
{
    if (Controller)
    {
        ARocketquakePlayerController *RocketQuakeController = Cast<ARocketquakePlayerController>(Controller.Get());

        if (RocketQuakeController)
        {
            RocketQuakeController->StartSpectating();
        }
    }
}

void ARocketQuakeCharacter::Client_OnHealthChanged_Implementation()
{
}

void ARocketQuakeCharacter::Multicast_ToggleSprint_Implementation()
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

void ARocketQuakeCharacter::Multicast_SpawnNiagaraSpawnSystemOnSpawn_Implementation()
{
    auto SpawnSystemEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
       GetWorld(),
       NiagaraSpawnSystemEffect,
       GetActorLocation(),
       GetActorRotation());

    if (SpawnSystemEffect)
    {
        SetNiagaraSkeletalMesh(SpawnSystemEffect);
    }
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
    DOREPLIFETIME(ARocketQuakeCharacter, bIsFirstTimeTick);
}
