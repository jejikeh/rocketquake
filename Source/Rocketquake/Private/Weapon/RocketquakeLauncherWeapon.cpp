// Rocketquake, jejikeh


#include "Weapon/RocketquakeLauncherWeapon.h"
#include "Weapon/LauncherProjectile.h"

void ARocketquakeLauncherWeapon::StartShoot()
{
    MakeShot();
}

void ARocketquakeLauncherWeapon::MakeShot()
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

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - WeaponMesh->GetSocketLocation("WeaponSocket")).GetSafeNormal();
    
    const FTransform Transform(FRotator::ZeroRotator, WeaponMesh->GetSocketTransform("WeaponSocket").GetLocation());
    Server_SpawnLauncherProjectile(Transform, Direction);

    DecreaseAmmo();
}

void ARocketquakeLauncherWeapon::Server_SpawnLauncherProjectile_Implementation(const FTransform Transform, const FVector Direction)
{
    if (ALauncherProjectile* Projectile = GetWorld()->SpawnActorDeferred<ALauncherProjectile>(LauncherProjectileClass, Transform))
    {
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(Transform);
        Projectile->SetShootDirection(Direction);
    }
}