// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Pickup/RocketquakePickup.h"
#include "PowerPickup.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API APowerPickup : public ARocketquakePickup
{
	GENERATED_BODY()

public:
    virtual bool GivePickupTo(APawn *PlayerPawn) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "10.0", ClampMax = "100.0"))
    TSubclassOf<class UGameplayEffect> DefaultAttributeEffects;
};
