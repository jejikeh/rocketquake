// Rocketquake, jejikeh


#include "Pickup/AmmoPickup.h"

#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"

bool AAmmoPickup::GivePickupTo(APawn *PlayerPawn)
{
    if (HasAuthority() && PlayerPawn)
    {
        const auto HealthComponent = Cast<UHealthComponent>(PlayerPawn->GetComponentByClass(UHealthComponent::StaticClass()));
        if (!HealthComponent || HealthComponent->IsDead())
        {
            return false;
        }

        const auto WeaponComponent = Cast<UWeaponComponent>(PlayerPawn->GetComponentByClass(UWeaponComponent::StaticClass()));
        if (!WeaponComponent)
        {
            return false;
        }

        return WeaponComponent->TryToAddAmmo(WeaponType, ClipsAmount);
    }
    
    return false;
}
