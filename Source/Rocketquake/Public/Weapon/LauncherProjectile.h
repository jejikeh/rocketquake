// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LauncherProjectile.generated.h"

class UWeaponFXComponent;
class URadialForceComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ROCKETQUAKE_API ALauncherProjectile : public AActor
{
    GENERATED_BODY()

public:
    ALauncherProjectile();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    USphereComponent* SphereComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    URadialForceComponent* RadialForceComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    UWeaponFXComponent* WeaponFXComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float DamageFallOff;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float OuterRadius;

    UFUNCTION(NetMulticast, Reliable)
    void SetShootDirection(const FVector& Direction);
    void SetShootDirection_Implementation(const FVector& Direction);

    UFUNCTION(NetMulticast, Reliable)
    void HandleHit(FHitResult Hit);
    void HandleHit_Implementation(FHitResult Hit);

    UFUNCTION(Server, Reliable)
    void Server_ApplyDamageToActors(FVector Locaction);
    void Server_ApplyDamageToActors_Implementation(FVector Locaction);

protected:
    UFUNCTION()
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);
    
    FVector ShootDirection;

private:
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
};
