// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


class ARocketquakeWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKETQUAKE_API UWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponComponent();

    UFUNCTION(Server, Reliable)
    void StartShoot();
    void StartShoot_Implementation();

    UFUNCTION(Server, Reliable)
    void StopShoot();
    void StopShoot_Implementation();

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ARocketquakeWeapon> WeaponClass;

private:
    UFUNCTION(Server, Reliable)
    void SpawnWeapon();
    void SpawnWeapon_Implementation();
    
    UPROPERTY()
    ARocketquakeWeapon* CurrentWeapon;
};
