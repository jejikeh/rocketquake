// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKETQUAKE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    UFUNCTION(BlueprintCallable)
    float GetHealth() const
    {
        return Health;
    };

    UFUNCTION(BlueprintCallable)
    bool IsDead() const
    {
        return FMath::IsNearlyZero(Health);
    };

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercent() const
    {
        return Health / MaxHealth;
    };

    FOnDeath OnDeath;

    FOnHealthChanged OnHealthChanged;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta=(ClampMin = "0.0", ClampMax = "100.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    bool bAutoHeal = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    float HealSpeedRate = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    float HealDelay = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float HealModifier = 1.0f;

private:
    UPROPERTY(Replicated)
    float Health = 0.0f;

    FTimerHandle HealTimerHandle;

    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *InstigatedBy,
        AActor *DamageCauser);

    void HealthHealUpdate();
};
