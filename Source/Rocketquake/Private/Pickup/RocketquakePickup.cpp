// Rocketquake, jejikeh


#include "Pickup/RocketquakePickup.h"

#include "Components/SphereComponent.h"

// Sets default values
ARocketquakePickup::ARocketquakePickup()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComponent->InitSphereRadius(50.0f);
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Overlap);
    SetRootComponent(SphereComponent);
}

void ARocketquakePickup::BeginPlay()
{
    Super::BeginPlay();
}

void ARocketquakePickup::NotifyActorBeginOverlap(AActor *OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    const auto Pawn = Cast<APawn>(OtherActor);
    if (HasAuthority() && GivePickupTo(Pawn))
    {
        PickupWasCollected();
    }
}

bool ARocketquakePickup::GivePickupTo(APawn *PlayerPawn)
{
    return false;
}

void ARocketquakePickup::PickupWasCollected_Implementation()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetRootComponent()->SetVisibility(false, true);

    FTimerHandle RespawnTimerHandle;
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ARocketquakePickup::Respawn, RespawnTime, false);
}

void ARocketquakePickup::Respawn()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
    GetRootComponent()->SetVisibility(true, true);
}
