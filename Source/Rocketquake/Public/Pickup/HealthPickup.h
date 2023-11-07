// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Pickup/RocketquakePickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API AHealthPickup : public ARocketquakePickup
{
    GENERATED_BODY()

protected:
    virtual bool GivePickupTo(APawn *PlayerPawn) override;
};
