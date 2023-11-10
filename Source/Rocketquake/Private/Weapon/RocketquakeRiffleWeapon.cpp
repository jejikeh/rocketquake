// Rocketquake, jejikeh


#include "Weapon/RocketquakeRiffleWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/WeaponFXComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"

ARocketquakeRiffleWeapon::ARocketquakeRiffleWeapon()
{
    WeaponFXComponent = CreateDefaultSubobject<UWeaponFXComponent>("WeaponFXComponent");
}

void ARocketquakeRiffleWeapon::Client_PlayCameraShake_Implementation()
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

void ARocketquakeRiffleWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void ARocketquakeRiffleWeapon::InitMuzzleFX_Implementation()
{
    if (!MuzzleFComponent)
    {
        MuzzleFComponent = SpawnMuzzleFlash();
    }

    SetMuzzleFXVisibility(true);
}

void ARocketquakeRiffleWeapon::SetMuzzleFXVisibility_Implementation(bool bVisible)
{
    if (MuzzleFComponent)
    {
        MuzzleFComponent->SetPaused(!bVisible);
        MuzzleFComponent->SetVisibility(bVisible, true);
    }
}

void ARocketquakeRiffleWeapon::Multicast_SpawnTraceFx_Implementation(FVector TraceEnd)
{
    const auto TraceFxComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        TraceFX,
        WeaponMesh->GetSocketLocation("MuzzleFlashSocket"));

    if (TraceFxComponent)
    {
        TraceFxComponent->SetNiagaraVariableVec3(TraceTarget, TraceEnd);
    }
}

void ARocketquakeRiffleWeapon::StartShoot()
{
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ARocketquakeRiffleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
    InitMuzzleFX();
}

void ARocketquakeRiffleWeapon::StopShoot()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetMuzzleFXVisibility(false);
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

    FVector TraceFxEnd = TraceEnd;

    if (HitResult.bBlockingHit)
    {
        TraceFxEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
        Multicast_DebugDraw(HitResult);
    }

    DecreaseAmmo();
    Client_PlayCameraShake();
    Multicast_SpawnTraceFx(TraceFxEnd);
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
    WeaponFXComponent->PlayImpactFx(HitResult);
    
    // DrawDebugLine(GetWorld(), WeaponMesh->GetSocketLocation("MuzzleFlashSocket"), HitResult.Location, FColor::Green, false, 3.0f);
    // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 3.0f);
}
