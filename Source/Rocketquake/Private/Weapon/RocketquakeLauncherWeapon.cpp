// Rocketquake, jejikeh


#include "Weapon/RocketquakeLauncherWeapon.h"
#include "Weapon/LauncherProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


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
    
    const FTransform Transform(FRotator::ZeroRotator, WeaponMesh->GetSocketTransform("WeaponSocket").GetLocation() + Direction * 100.0f);
    Server_SpawnLauncherProjectile(Transform, Direction);

    Server_DecreaseAmmo();

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    if (Player->IsLocallyControlled() && !HasAuthority())
    {
        Server_DecreaseAmmo_Implementation();
    }
    
    Client_PlayCameraShake();
    Multicast_InitNiagaraSystem();
}

void ARocketquakeLauncherWeapon::Multicast_InitNiagaraSystem_Implementation()
{
    SpawnMuzzleFlash();
    UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, "MuzzleFlashSocket");
}

void ARocketquakeLauncherWeapon::Server_SpawnLauncherProjectile_Implementation(const FTransform Transform, const FVector Direction)
{
    if (IsAmmoEmpty())
    {
        return;
    }
    
    if (ALauncherProjectile* Projectile = GetWorld()->SpawnActorDeferred<ALauncherProjectile>(LauncherProjectileClass, Transform))
    {
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(Transform);
        Projectile->SetShootDirection(Direction);
    }
}
