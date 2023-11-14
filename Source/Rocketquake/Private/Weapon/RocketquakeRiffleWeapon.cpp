// Rocketquake, jejikeh


#include "Weapon/RocketquakeRiffleWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/WeaponFXComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

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

void ARocketquakeRiffleWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (HasAuthority())
    {
        StopShoot();
    }

    SetMuzzleFXVisibility_Implementation(false);
    
    Super::EndPlay(EndPlayReason);
}

void ARocketquakeRiffleWeapon::InitMuzzleFX_Implementation()
{
    if (!MuzzleFComponent)
    {
        MuzzleFComponent = SpawnMuzzleFlash();
    }

    if (!FireAudioComponent)
    {
        FireAudioComponent = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, "MuzzleFlashSocket");
    }

    SetMuzzleFXVisibility_Implementation(true);
}

void ARocketquakeRiffleWeapon::SetMuzzleFXVisibility_Implementation(bool bVisible)
{
    if (!IsValidLowLevelFast())
    {
        return;
    }
    
    if (MuzzleFComponent)
    {
        MuzzleFComponent->SetPaused(!bVisible);
        MuzzleFComponent->SetVisibility(bVisible, true);
    }

    if (FireAudioComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("bVisible: %d"), bVisible);
        bVisible ? FireAudioComponent->Play() : FireAudioComponent->Stop();
    }
}

void ARocketquakeRiffleWeapon::Multicast_SpawnTraceFx_Implementation(FVector TraceEnd)
{
    if (!IsValidLowLevelFast())
    {
        return;
    }
    
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

    if (IsValidLowLevelFast())
    {
        SetMuzzleFXVisibility(false);
    }
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
}
