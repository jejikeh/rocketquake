// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/GameMatchStates.h"
#include "PlayerHudWidget.generated.h"

struct FAmmoData;
struct FWeaponUIData;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UPlayerHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool GetWeaponUIData(FWeaponUIData& WeaponUIData) const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool GetWeaponAmmoData(FAmmoData& AmmoData) const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsPlayerAlive();

    UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsPlayerSpectating();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnTakeDamage(float Damage);

    UFUNCTION()
    void OnKillPlayer(FString Victim);
    
    void NativeOnInitialized() override;

private:
    float SavedHealth;
};
