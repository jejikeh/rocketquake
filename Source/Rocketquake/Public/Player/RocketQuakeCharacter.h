// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RocketQuakeCharacter.generated.h"

class ARocketquakeWeapon;
struct FInputActionValue;

UCLASS()
class ROCKETQUAKE_API ARocketQuakeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARocketQuakeCharacter(const FObjectInitializer &ObjectInitializer);
    
    virtual void Tick(float DeltaTime) override;
    
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsSprinting() const;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    class USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    class UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    class UTextRenderComponent* TextRenderComponent;

private:
    UPROPERTY(ReplicatedUsing = OnRep_ToggleSprint)
    bool bIsSprinting;

    UPROPERTY(Replicated)
    bool bIsMovingForward;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* MoveForwardAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* MoveRightAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess = "true"))
    class UInputAction* SprintAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.0", ClampMax = "10.0", AllowPrivateAccess = "true"))
    float SprintModifier = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ARocketquakeWeapon> WeaponClass;

    float BaseWalkSpeed;

    void MoveForwardCharacter(const FInputActionValue& Value);

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

    UFUNCTION(Client, Reliable)
    void Client_OnHealthChanged();
    void Client_OnHealthChanged_Implementation();

    void SpawnWeapon();

    void HandleStartSprintAction();

    void HandleStopSprintAction();
};
