// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Weapon/RocketquakeWeapon.h"
#include "RocketquakeRiffleWeapon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
struct FWeaponData;
class UWeaponFXComponent;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakeRiffleWeapon : public ARocketquakeWeapon
{
    GENERATED_BODY()

public:
    ARocketquakeRiffleWeapon();
    
    virtual void StartShoot() override;

    virtual void StopShoot() override;

    virtual void ForceStopShot() override;

    void SmoothEnableFov();
    
    virtual void Zoom(bool Enabled) override;

protected:
    virtual void MakeShot() override;

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    virtual void MakeDamage(const FHitResult& HitResult);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TimeBetweenShots = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    UWeaponFXComponent* WeaponFXComponent;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UNiagaraSystem* TraceFX;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    FString TraceTarget = "Vector";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FovZoomAngle = 30.0f; 
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float BulletSpread = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float ZoomCameraLagSpeed = 1.0f;
    
    FTimerHandle InZoomTimerHandle;

    virtual void ResetZoom() override;
    
    UFUNCTION(Server, Reliable)
    void Server_StartShoot();
    void Server_StartShoot_Implementation();

    void Local_StartShoot();

    void Local_StopShoot();

    UFUNCTION(Server, Reliable)
    void Server_StopShoot();
    void Server_StopShoot_Implementation();
    
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_StartShoot();
    void Multicast_StartShoot_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_StopShoot();
    void Multicast_StopShoot_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ForceStopShoot();
    void Multicast_ForceStopShoot_Implementation();

    UFUNCTION(Server, Reliable)
    void Server_ForceStopShoot();
    void Server_ForceStopShoot_Implementation();
    
    void PlayCameraShake();

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    FTimerHandle ShotTimerHandle;

    UPROPERTY()
    UNiagaraComponent* MuzzleFComponent;

    UPROPERTY()
    UAudioComponent* FireAudioComponent;

    UFUNCTION(NetMulticast, Unreliable)
    void InitMuzzleFX();
    void InitMuzzleFX_Implementation();

    UFUNCTION(NetMulticast, Unreliable)
    void SetMuzzleFXVisibility(bool bVisible);
    void SetMuzzleFXVisibility_Implementation(bool bVisible);

    void SpawnTraceFx(const FVector &TraceEnd) const;

    void SetMouseZoomSensitivity(bool bZoom);

    float DefaultCameraFov = 90.0f;

    float DesiredCameraFov = 90.0f;

    float DefaultMouseSensitivity = 1.0f;

    float DefaultCameraLagSpeed = 5.0f;

    float DesiredCameraLagSpeed = 5.0f;
};
