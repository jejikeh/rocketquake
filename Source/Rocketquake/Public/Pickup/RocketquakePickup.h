// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketquakePickup.generated.h"

class USphereComponent;

UCLASS()
class ROCKETQUAKE_API ARocketquakePickup : public AActor
{
    GENERATED_BODY()

public:
    ARocketquakePickup();

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, Category = "Pickup")
    USphereComponent* SphereComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
    float RespawnTime = 15.0f;

    virtual void NotifyActorBeginOverlap(AActor *OtherActor) override;

    virtual bool GivePickupTo(APawn *PlayerPawn);

private:
    UFUNCTION(NetMulticast, Reliable)
    void PickupWasCollected();
    void PickupWasCollected_Implementation();
    
    void Respawn();

    float RotationYaw = 0.0f;

    void GenerateRotationYaw();
};
