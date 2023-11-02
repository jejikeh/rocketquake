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

    virtual void StartShoot();
    virtual void StopShoot();

protected:
    virtual void BeginPlay() override;

    void MakeHit(FHitResult& HitResult, const FVector& Vector, const FVector& TraceEnd) const;

    virtual void MakeShot();

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    AController* GetController() const;

    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float Damage = 10.0f;
};
