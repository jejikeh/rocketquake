// Rocketquake, jejikeh


#include "GAS/GASAttributeSet.h"

#include "Net/UnrealNetwork.h"

UGASAttributeSet::UGASAttributeSet()
{
    
}

void UGASAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Power, COND_None, REPNOTIFY_Always);
}

void UGASAttributeSet::OnRep_Health(const FGameplayAttributeData &OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Health, OldHealth);
}

void UGASAttributeSet::OnRep_Power(const FGameplayAttributeData &OldPower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Power, OldPower);
}
