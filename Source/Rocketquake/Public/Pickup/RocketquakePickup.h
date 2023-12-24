// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketquakePickup.generated.h"

class USoundCue;
class UNiagaraSystem;
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

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UNiagaraSystem* PickupFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
    float RespawnTime = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
    USoundCue* PickupSound;

    virtual void NotifyActorBeginOverlap(AActor *OtherActor) override;

    virtual bool GivePickupTo(APawn *PlayerPawn);
    
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_SpawnNiagaraSpawnSystemOnSpawn();
    void Multicast_SpawnNiagaraSpawnSystemOnSpawn_Implementation();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX", meta = (AllowPrivateAccess = "true"))
    UNiagaraSystem* NiagaraSpawnSystemEffect;

private:
    UFUNCTION(NetMulticast, Reliable)
    void PickupWasCollected();
    void PickupWasCollected_Implementation();
    
    void Respawn();

    float RotationYaw = 0.0f;

    void GenerateRotationYaw();

    bool bIsFirstTimeTick = true;
};
