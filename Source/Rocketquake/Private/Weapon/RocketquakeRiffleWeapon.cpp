// Rocketquake, jejikeh


#include "Weapon/RocketquakeRiffleWeapon.h"

#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"

void ARocketquakeRiffleWeapon::StartShoot()
{
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ARocketquakeRiffleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ARocketquakeRiffleWeapon::StopShoot()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ARocketquakeRiffleWeapon::MakeShot()
{
    if (IsAmmoEmpty())
    {
        return;
    }
    
    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        return;
    }

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    if (HitResult.bBlockingHit)
    {
        MakeDamage(HitResult);
        Multicast_DebugDraw(HitResult);
    }

    DecreaseAmmo();
}

bool ARocketquakeRiffleWeapon::GetTraceData(FVector &TraceStart, FVector &TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
    {
        return false;
    }

    TraceStart = ViewLocation;
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    const auto ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad);
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    return true;
}

void ARocketquakeRiffleWeapon::MakeDamage(const FHitResult &HitResult)
{
    const auto DamageActor = Cast<ACharacter>(HitResult.GetActor());
    if (!DamageActor)
    {
        return;
    }

    DamageActor->TakeDamage(Damage, FDamageEvent(), GetController(), this);
}

void ARocketquakeRiffleWeapon::Multicast_DebugDraw_Implementation(FHitResult HitResult)
{
    DrawDebugLine(GetWorld(), WeaponMesh->GetSocketLocation("WeaponSocket"), HitResult.Location, FColor::Green, false, 3.0f);
    DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 3.0f);
}
