// Rocketquake, jejikeh


#include "Pickup/PowerPickup.h"

#include "GAS/GASAbilitySystemComponent.h"

bool APowerPickup::GivePickupTo(APawn *PlayerPawn)
{
    if (PlayerPawn)
    {
        const auto AttributesComponent = Cast<UGASAbilitySystemComponent>(PlayerPawn->GetComponentByClass(UGASAbilitySystemComponent::StaticClass()));
        if (!AttributesComponent)
        {
            return false;
        }

        FGameplayEffectContextHandle EffectContext = AttributesComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);
        const FGameplayEffectSpecHandle SpecHandle = AttributesComponent->MakeOutgoingSpec(DefaultAttributeEffects, 1, EffectContext);
        AttributesComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        
        return true;
    }

    return false;
}
