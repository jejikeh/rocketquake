// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketquakeWeapon.generated.h"

UCLASS()
class ROCKETQUAKE_API ARocketquakeWeapon : public AActor
{
    GENERATED_BODY()

public:
    ARocketquakeWeapon();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;
};
