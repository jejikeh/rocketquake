// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Character/PBPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <GameplayEffectTypes.h>

#include "Rocketquake/Rocketquake.h"
#include "RocketQuakeCharacter.generated.h"

class USoundCue;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ARocketquakeWeapon;
struct FInputActionValue;

UCLASS()
class ROCKETQUAKE_API ARocketQuakeCharacter : public APBPlayerCharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer);
    
    virtual void Tick(float DeltaTime) override;

    void MoveForwardStarted(const FInputActionValue & InputActionValue);

    void StartZoom();

    void StopZoom();

    void StartPunch();
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool GetIsSprinting() const;

    virtual bool IsSprinting() const override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

    UFUNCTION(NetMulticast, Reliable)
    void SetPlayerColor(FLinearColor NewColor);
    void SetPlayerColor_Implementation(FLinearColor NewColor);

    UFUNCTION(BlueprintCallable)
    FRotator GetLastReplicatedViewRotation();

    UFUNCTION(BlueprintCallable)
    void ResetAnimNotifies();

    float GetCameraPitch() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float MouseSensitivity;

    void CameraLagSpeed(float Speed);

    float GetCameraLagSpeed() const;

protected:
    UFUNCTION()
    void OnCameraCollisionBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    void CheckCameraOverlap() const;
    
    UFUNCTION()
    void OnCameraCollisionEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnCharacterMovementRight(float Value);

    UFUNCTION(BlueprintImplementableEvent)
    void SetNiagaraSkeletalMesh(UNiagaraComponent *NiagaraComponent);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    class UCameraComponent* CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    class UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class USpringArmComponent* SpringArmComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class USpringArmComponent* FirstPersonArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    class UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    class UWeaponComponent *WeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    USphereComponent* CameraCollisionSphereComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UGASAbilitySystemComponent* GASAbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UGASAttributeSet* GASAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    FRotator BaseAimRotationShift;

    virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void InitializeAttributes();
    virtual void GiveAbilities();

    // Call on server, use to setup GAS on character
    virtual void PossessedBy(AController *NewController) override;

    // Call on client, use to setup GAS on character
    virtual void OnRep_PlayerState() override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|Attributes")
    TSubclassOf<class UGameplayEffect> DefaultAttributeEffects;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|Attributes")
    TArray<TSubclassOf<class UGASGameplayAbility>> DefaultAbilities;

private:
    UPROPERTY(ReplicatedUsing = OnRep_ToggleSprint)
    bool bIsSprinting;

    UPROPERTY(ReplicatedUsing = OnRep_ToggleSprint)
    bool bIsMovingForward;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* MoveForwardAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    UInputAction* MoveRightAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    UInputAction* SprintAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* ShootAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* SwitchWeaponAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* ReloadAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* SwitchCameraViewAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* ZoomAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* PunchAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.0", ClampMax = "10.0", AllowPrivateAccess = "true"))
    float SprintModifier = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Materials", meta = (AllowPrivateAccess = "true"))
    FName MaterialColorName = "Paint Color";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX", meta = (AllowPrivateAccess = "true"))
    UNiagaraSystem* NiagaraSpawnSystemEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
    USoundCue* DeathSound;

    float BaseWalkSpeed;

    void SendLocalInputToGAS(bool bIsPressed, EGASAbilityInputID AbilityInputID);

    void MoveForwardCharacter(const FInputActionValue& Value);

    void ResetMoveForwardCharacter(const FInputActionValue& Value);
    
    void MoveRightCharacter(const FInputActionValue& Value);
    
    void LookCharacter(const FInputActionValue& Value);
    
    UFUNCTION(Server, Reliable)
    void Server_SetSprint(bool Sprinting);
    void Server_SetSprint_Implementation(bool Sprinting);
    
    UFUNCTION(Server, Reliable)
    void Server_SetMovingForward(bool IsMovingForward);
    void Server_SetMovingForward_Implementation(bool IsMovingForward);

    UFUNCTION()
    void OnRep_ToggleSprint();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnDeath();
    void Multicast_OnDeath_Implementation();

    UFUNCTION(Server, Reliable)
    void Server_OnDeath();
    void Server_OnDeath_Implementation();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_SpawnNiagaraSpawnSystemOnSpawn();
    void Multicast_SpawnNiagaraSpawnSystemOnSpawn_Implementation();
    
    void HandleStartSprintAction();

    void HandleStopSprintAction();

    void SwitchCameraView();

    bool bIsThirdPerson = true;

    UPROPERTY(Replicated)
    bool bIsFirstTimeTick = true;
};
