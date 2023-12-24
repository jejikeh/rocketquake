// Rocketquake, jejikeh


#include "Player/RocketQuakeCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RocketquakeAdvancedGameInstance.h"
#include "RocketquakeGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RocketquakeMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/GASAbilitySystemComponent.h"
#include "GAS/GASAttributeSet.h"
#include "GAS/GASGameplayAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/RocketquakePlayerController.h"

ARocketQuakeCharacter::ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<URocketquakeMovementComponent>(CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetMesh(), "CameraSocket");
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("ThirdPersonCameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bAutoActivate = true;

    FirstPersonArmComponent = CreateDefaultSubobject<USpringArmComponent>("FirstPersonArmComponent");
    FirstPersonArmComponent->SetupAttachment(GetMesh(), "CameraSocket");
    FirstPersonArmComponent->bUsePawnControlRotation = true;
    
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>("FirstPersonCameraComponent");
    FirstPersonCameraComponent->SetupAttachment(FirstPersonArmComponent, USpringArmComponent::SocketName);
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->bAutoActivate = false;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
    HealthComponent->SetIsReplicated(true);
    HealthComponent->SetNetAddressable();

    CameraCollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CameraCollisionSphereComponent"));
    CameraCollisionSphereComponent->SetupAttachment(CameraComponent);
    CameraCollisionSphereComponent->SetSphereRadius(15.0f);
    CameraCollisionSphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>("WeaponComponent");
    WeaponComponent->SetIsReplicated(true);
    WeaponComponent->SetNetAddressable();

    GASAbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>("AbilitySystemComponent");
    GASAbilitySystemComponent->SetIsReplicated(true);
    GASAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    GASAttributeSet = CreateDefaultSubobject<UGASAttributeSet>("GASAttributeSet");
}

void ARocketQuakeCharacter::BeginPlay()
{
    Super::BeginPlay();

    BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    HealthComponent->OnDeath.AddDynamic(this, &ARocketQuakeCharacter::Multicast_OnDeath);

    CameraCollisionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketQuakeCharacter::OnCameraCollisionBeginOverlap);
    CameraCollisionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ARocketQuakeCharacter::OnCameraCollisionEndOverlap);

    if (const auto PlayerController = Cast<ARocketquakePlayerController>(GetController()))
    {
        if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    MouseSensitivity = Cast<URocketquakeAdvancedGameInstance>(GetGameInstance())->GetMouseSensitivity();
}

UAbilitySystemComponent* ARocketQuakeCharacter::GetAbilitySystemComponent() const
{
    return GASAbilitySystemComponent;
}

void ARocketQuakeCharacter::InitializeAttributes()
{
    if (GASAbilitySystemComponent && DefaultAttributeEffects)
    {
        FGameplayEffectContextHandle EffectContext = GASAbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        const FGameplayEffectSpecHandle SpecHandle = GASAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffects, 1, EffectContext);
        if (SpecHandle.IsValid())
        {
            GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

void ARocketQuakeCharacter::GiveAbilities()
{
    if (HasAuthority() && GASAbilitySystemComponent)
    {
        for (TSubclassOf<UGASGameplayAbility>& Ability : DefaultAbilities)
        {
            GASAbilitySystemComponent->GiveAbility(
                FGameplayAbilitySpec(
                    Ability,
                    1,
                    static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID),
                    this));
        }
    }
}

void ARocketQuakeCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);
    
    GASAbilitySystemComponent->InitAbilityActorInfo(this, this);

    InitializeAttributes();
    GiveAbilities();
}

void ARocketQuakeCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    GASAbilitySystemComponent->InitAbilityActorInfo(this, this);
    InitializeAttributes();
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
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::MoveForwardStarted);
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveForwardCharacter);
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this,
            &ARocketQuakeCharacter::ResetMoveForwardCharacter);

        EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::MoveRightCharacter);

        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARocketQuakeCharacter::LookCharacter);

        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::StopJumping);

        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::HandleStartSprintAction);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::HandleStopSprintAction);

        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::StartShoot_Implementation);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, WeaponComponent, &UWeaponComponent::StopShoot_Implementation);

        EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::NextWeapon_Implementation);

        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, WeaponComponent, &UWeaponComponent::Reload_Implementation);

        EnhancedInputComponent->BindAction(SwitchCameraViewAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::SwitchCameraView);
        
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::StartZoom);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ARocketQuakeCharacter::StopZoom);

        EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &ARocketQuakeCharacter::StartPunch);
    }
}

bool ARocketQuakeCharacter::GetIsSprinting() const
{
    return IsSprinting();
}

bool ARocketQuakeCharacter::IsSprinting() const
{
    return bIsSprinting && bIsMovingForward;
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

FRotator ARocketQuakeCharacter::GetLastReplicatedViewRotation()
{
    auto BaseAimRotation = GetBaseAimRotation();
    if (!IsLocallyControlled() && BaseAimRotation.Pitch > 90 && BaseAimRotation.Pitch < 360) {
        // NOTE: Hack to fix animations on BS movement pitch
        BaseAimRotation.Pitch -= 360;
    }
    
    if (!HasAuthority() && !IsLocallyControlled())
    {
        // NOTE: Hack to fix the camera rotation on clients
        BaseAimRotation += BaseAimRotationShift;
    }

    return BaseAimRotation;
}

void ARocketQuakeCharacter::ResetAnimNotifies()
{
    if (HasAuthority() && WeaponComponent)
    {
        WeaponComponent->Multicast_ResetAnimNotifies();
    }
}

float ARocketQuakeCharacter::GetCameraPitch() const
{
    return CameraComponent->GetRelativeRotation().Pitch;
}

void ARocketQuakeCharacter::CameraLagSpeed(float Speed)
{
    SpringArmComponent->CameraLagSpeed = Speed;
}

float ARocketQuakeCharacter::GetCameraLagSpeed() const
{
    return SpringArmComponent->CameraLagSpeed;
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

void ARocketQuakeCharacter::MoveForwardStarted(const FInputActionValue &InputActionValue)
{
    const auto MovementVector = InputActionValue.Get<FVector2D>();
    Server_SetMovingForward(MovementVector.X < 0.0f);
}

void ARocketQuakeCharacter::StartZoom()
{
    if (FirstPersonCameraComponent->IsActive())
    {
        return;
    }
    
    WeaponComponent->Zoom(true);
}

void ARocketQuakeCharacter::StopZoom()
{
    if (FirstPersonCameraComponent->IsActive())
    {
        return;
    }
    
    WeaponComponent->Zoom(false);
}

void ARocketQuakeCharacter::StartPunch()
{
    if (!GASAbilitySystemComponent)
    {
        UKismetSystemLibrary::PrintString(this, "GAS Ability System Component not found!");
    }

    if (GASAttributeSet->GetPower() <= 0.0f)
    {
        return;
    }
    
    SendLocalInputToGAS(true, EGASAbilityInputID::Punch);
}

void ARocketQuakeCharacter::SendLocalInputToGAS(bool bIsPressed, EGASAbilityInputID AbilityInputID)
{
    if (!GASAbilitySystemComponent)
    {
        UKismetSystemLibrary::PrintString(this, "GAS Ability System Component not found!");
    }

    if (bIsPressed)
    {
        GASAbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
    }
    else
    {
        GASAbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
    }
}

void ARocketQuakeCharacter::MoveForwardCharacter(const FInputActionValue &Value)
{
    const auto MovementVector = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), -MovementVector.X);
}

void ARocketQuakeCharacter::ResetMoveForwardCharacter(const FInputActionValue &Value)
{
    Server_SetMovingForward(false);
}

void ARocketQuakeCharacter::MoveRightCharacter(const FInputActionValue &Value)
{
    const auto MovementVector = Value.Get<FVector2D>();
    AddMovementInput(GetActorRightVector(), MovementVector.X);
    OnCharacterMovementRight(MovementVector.X);
}

void ARocketQuakeCharacter::LookCharacter(const FInputActionValue &Value)
{
    const auto LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(-LookVector.X * MouseSensitivity);
    AddControllerPitchInput(LookVector.Y * MouseSensitivity);
}

void ARocketQuakeCharacter::Server_SetMovingForward_Implementation(bool IsMovingForward)
{
    bIsMovingForward = IsMovingForward;
    OnRep_ToggleSprint();
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
    WeaponComponent->ResetZoom();
    
    GetCharacterMovement()->DisableMovement();
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

    if (IsLocallyControlled())
    {
        Server_OnDeath();
    }
}

void ARocketQuakeCharacter::Server_OnDeath_Implementation()
{
    if (Controller)
    {
        if (ARocketquakePlayerController *RocketQuakeController = Cast<ARocketquakePlayerController>(Controller.Get()))
        {
            RocketQuakeController->StartSpectating();
        }
    }

    SetLifeSpan(5.0f);
    WeaponComponent->GetCurrentWeapon()->SetLifeSpan(5.0f);
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

void ARocketQuakeCharacter::SwitchCameraView()
{
    if (FirstPersonCameraComponent->IsActive() == CameraComponent->IsActive())
    {
        CameraComponent->SetActive(false);
        FirstPersonCameraComponent->SetActive(true);
        bIsThirdPerson = CameraComponent->IsActive();
        return;
    }

    FirstPersonCameraComponent->SetActive(!FirstPersonCameraComponent->IsActive());
    CameraComponent->SetActive(!CameraComponent->IsActive());
    bIsThirdPerson = CameraComponent->IsActive();
    WeaponComponent->Zoom(false);
}

void ARocketQuakeCharacter::Server_SetSprint_Implementation(bool Sprinting)
{
    bIsSprinting = Sprinting;
    OnRep_ToggleSprint();
}

void ARocketQuakeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARocketQuakeCharacter, bIsSprinting);
    DOREPLIFETIME(ARocketQuakeCharacter, bIsMovingForward);
    DOREPLIFETIME(ARocketQuakeCharacter, bIsFirstTimeTick);
}
