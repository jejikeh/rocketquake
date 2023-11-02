// Rocketquake, jejikeh


#include "Weapon/RocketquakeWeapon.h"

#include "GameFramework/Character.h"

// Sets default values
ARocketquakeWeapon::ARocketquakeWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

void ARocketquakeWeapon::StartShoot()
{
}

void ARocketquakeWeapon::StopShoot()
{
}

void ARocketquakeWeapon::MakeShot()
{
}

void ARocketquakeWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void ARocketquakeWeapon::MakeHit(FHitResult &HitResult, const FVector &Vector, const FVector &TraceEnd) const
{
    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());

    GetWorld()->LineTraceSingleByChannel(HitResult, Vector, TraceEnd, ECC_Visibility, CollisionQueryParams);
}

bool ARocketquakeWeapon::GetTraceData(FVector &TraceStart, FVector &TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
    {
        return false;
    }

    TraceStart = ViewLocation;
    TraceEnd = TraceStart + ViewRotation.Vector() * TraceMaxDistance;

    return true;
}

AController *ARocketquakeWeapon::GetController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return nullptr;
    }

    const auto Controller = Cast<APlayerController>(Player->GetController());
    if (!Controller)
    {
        return nullptr;
    }

    return Controller;
}

bool ARocketquakeWeapon::GetPlayerViewPoint(FVector &ViewLocation, FRotator &ViewRotation) const
{
    const auto Controller = GetController();
    if (!Controller)
    {
        return false;
    }

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}
