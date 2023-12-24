// Rocketquake, jejikeh


#include "Pickup/HealthPickup.h"

#include "Components/HealthComponent.h"

bool AHealthPickup::GivePickupTo(APawn *PlayerPawn)
{
    if (PlayerPawn)
    {
        const auto HealthComponent = Cast<UHealthComponent>(PlayerPawn->GetComponentByClass(UHealthComponent::StaticClass()));
        return HealthComponent->AddHealth(HealthAmount);
    }

    return false;
}
