// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Pickup/RocketquakePickup.h"
#include "AmmoPickup.generated.h"

class ARocketquakeWeapon;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API AAmmoPickup : public ARocketquakePickup
{
    GENERATED_BODY()

protected:
    virtual bool GivePickupTo(APawn *PlayerPawn) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "1.0", ClampMax = "10.0"))
    int32 ClipsAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    TSubclassOf<ARocketquakeWeapon> WeaponType;
};
