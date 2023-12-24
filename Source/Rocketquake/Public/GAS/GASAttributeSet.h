// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GASAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UGASAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UGASAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Health attribute
    UPROPERTY(BlueprintReadOnly, Category = "GAS|Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UGASAttributeSet, Health)

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);

    // Power attribute
    UPROPERTY(BlueprintReadOnly, Category = "GAS|Attributes", ReplicatedUsing = OnRep_Power)
    FGameplayAttributeData Power;
    ATTRIBUTE_ACCESSORS(UGASAttributeSet, Power)

    UFUNCTION()
    void OnRep_Power(const FGameplayAttributeData& OldPower);
};
