// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Weapon/RocketquakeWeapon.h"
#include "RocketquakeRiffleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakeRiffleWeapon : public ARocketquakeWeapon
{
    GENERATED_BODY()

public:
    virtual void StartShoot() override;

    virtual void StopShoot() override;

protected:
    virtual void MakeShot() override;

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    virtual void MakeDamage(const FHitResult& HitResult);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TimeBetweenShots = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float BulletSpread = 1.0f;

private:
    FTimerHandle ShotTimerHandle;
};
