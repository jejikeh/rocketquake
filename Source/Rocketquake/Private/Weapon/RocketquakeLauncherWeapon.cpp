// Rocketquake, jejikeh


#include "Weapon/RocketquakeLauncherWeapon.h"
#include "Weapon/LauncherProjectile.h"
#include "GameFramework/Character.h"


void ARocketquakeLauncherWeapon::StartShoot()
{
    MakeShot();
}

void ARocketquakeLauncherWeapon::Client_PlayCameraShake_Implementation()
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller)
    {
        return;
    }

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
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
    Client_PlayCameraShake();
    Multicast_InitNiagaraSystem();
}

void ARocketquakeLauncherWeapon::Multicast_InitNiagaraSystem_Implementation()
{
    SpawnMuzzleFlash();
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
