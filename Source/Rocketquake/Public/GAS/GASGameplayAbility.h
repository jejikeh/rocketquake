// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Rocketquake/Rocketquake.h"
#include "GASGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UGASGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGASGameplayAbility();

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GAS|Ability")
    EGASAbilityInputID AbilityInputID = EGASAbilityInputID::None;
};
