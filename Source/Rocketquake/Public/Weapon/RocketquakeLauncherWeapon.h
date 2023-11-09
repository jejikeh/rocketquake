// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Weapon/RocketquakeWeapon.h"
#include "RocketquakeLauncherWeapon.generated.h"

class ALauncherProjectile;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakeLauncherWeapon : public ARocketquakeWeapon
{
	GENERATED_BODY()

public:
    virtual void StartShoot() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ALauncherProjectile> LauncherProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_InitNiagaraSystem();
    void Multicast_InitNiagaraSystem_Implementation();

    UFUNCTION(Server, Reliable)
    void Server_SpawnLauncherProjectile(const FTransform Transform, const FVector Direction);
    void Server_SpawnLauncherProjectile_Implementation(const FTransform Transform, const FVector Direction);
    
    UFUNCTION(Client, Reliable)
    void Client_PlayCameraShake();
    void Client_PlayCameraShake_Implementation();
    
    virtual void MakeShot() override;
};
