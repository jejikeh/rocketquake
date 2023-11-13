// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "RocketQuakeCharacter.generated.h"

class USoundCue;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ARocketquakeWeapon;
struct FInputActionValue;

UCLASS()
class ROCKETQUAKE_API ARocketQuakeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer);
    
    virtual void Tick(float DeltaTime) override;

    void MoveForwardStarted(const FInputActionValue & InputActionValue);
    
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsSprinting() const;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

    UFUNCTION(NetMulticast, Reliable)
    void SetPlayerColor(FLinearColor NewColor);
    void SetPlayerColor_Implementation(FLinearColor NewColor);

protected:
    UFUNCTION()
    void OnCameraCollisionBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    void CheckCameraOverlap() const;
    
    UFUNCTION()
    void OnCameraCollisionEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void SetNiagaraSkeletalMesh(UNiagaraComponent *NiagaraComponent);

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    class UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    class UWeaponComponent *WeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    USphereComponent* CameraCollisionSphereComponent;

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

    UPROPERTY(Replicated)
    bool bIsFirstTimeTick = true;
};
