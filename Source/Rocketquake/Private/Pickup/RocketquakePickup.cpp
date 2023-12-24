// Rocketquake, jejikeh


#include "Pickup/RocketquakePickup.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ARocketquakePickup::ARocketquakePickup()
{
    PrimaryActorTick.bCanEverTick = true;
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

    GenerateRotationYaw();
}

void ARocketquakePickup::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AddActorLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));

    if (bIsFirstTimeTick)
    {
        Multicast_SpawnNiagaraSpawnSystemOnSpawn();
        bIsFirstTimeTick = false;
    }
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

void ARocketquakePickup::Multicast_SpawnNiagaraSpawnSystemOnSpawn_Implementation()
{
    auto SpawnSystemEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
       GetWorld(),
       NiagaraSpawnSystemEffect,
       GetActorLocation(),
       GetActorRotation());
}

void ARocketquakePickup::PickupWasCollected_Implementation()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetRootComponent()->SetVisibility(false, true);

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupFX, GetActorLocation());

    FTimerHandle RespawnTimerHandle;
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ARocketquakePickup::Respawn, RespawnTime, false);

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}

void ARocketquakePickup::Respawn()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
    GetRootComponent()->SetVisibility(true, true);
    GenerateRotationYaw();
}

void ARocketquakePickup::GenerateRotationYaw()
{
    RotationYaw = FMath::RandRange(1.0f, 2.0f) * (FMath::RandBool() ? 1.0f : -1.0f);
}
