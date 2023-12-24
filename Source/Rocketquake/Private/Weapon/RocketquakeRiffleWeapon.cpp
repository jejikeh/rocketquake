// Rocketquake, jejikeh


#include "Weapon/RocketquakeRiffleWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/WeaponFXComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "RocketquakeAdvancedGameInstance.h"
#include "RocketquakeGameInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RocketQuakeCharacter.h"
#include "Player/RocketquakePlayerController.h"
#include "Sound/SoundCue.h"

ARocketquakeRiffleWeapon::ARocketquakeRiffleWeapon()
{
    WeaponFXComponent = CreateDefaultSubobject<UWeaponFXComponent>("WeaponFXComponent");
}

void ARocketquakeRiffleWeapon::PlayCameraShake()
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

    const auto Controller = Cast<ARocketquakePlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager)
    {
        return;
    }

    DefaultCameraFov = Controller->PlayerCameraManager->GetFOVAngle();

    const auto Character = Cast<ARocketQuakeCharacter>(Controller->GetCharacter());
    if (!Character)
    {
        return;
    }

    DefaultMouseSensitivity = Cast<URocketquakeAdvancedGameInstance>(GetGameInstance())->GetMouseSensitivity();
    DefaultCameraLagSpeed = Character->GetCameraLagSpeed();
}

void ARocketquakeRiffleWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopShoot();
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
        bVisible ? FireAudioComponent->Play() : FireAudioComponent->Stop();
    }
}

void ARocketquakeRiffleWeapon::SpawnTraceFx(const FVector &TraceEnd) const
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

void ARocketquakeRiffleWeapon::SetMouseZoomSensitivity(bool bZoom)
{
    const auto Controller = Cast<ARocketquakePlayerController>(GetController());
    if (!Controller)
    {
        return;
    }

    const auto Character = Cast<ARocketQuakeCharacter>(Controller->GetCharacter());
    if (!Character)
    {
        return;
    }
    
    if (bZoom)
    {
        Character->MouseSensitivity = DefaultMouseSensitivity / Cast<URocketquakeAdvancedGameInstance>(GetGameInstance())->GetSlowdownZoomFactor();
    }
    else
    {
        Character->MouseSensitivity = Cast<URocketquakeAdvancedGameInstance>(GetGameInstance())->GetMouseSensitivity();
    }
}

void ARocketquakeRiffleWeapon::StartShoot()
{
    Local_StartShoot();
    Server_StartShoot();
}

void ARocketquakeRiffleWeapon::StopShoot()
{
    Local_StopShoot();
    Server_StopShoot();
}

void ARocketquakeRiffleWeapon::ForceStopShot()
{
    Server_ForceStopShoot();
}

void ARocketquakeRiffleWeapon::Zoom(bool Enabled)
{
    const auto Controller = Cast<ARocketquakePlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager)
    {
        return;
    }

    SetMouseZoomSensitivity(Enabled);
    DesiredCameraFov = Enabled ? FovZoomAngle : DefaultCameraFov;
    DesiredCameraLagSpeed = Enabled ? ZoomCameraLagSpeed : DefaultCameraLagSpeed;

    GetWorldTimerManager().SetTimer(
            InZoomTimerHandle,
            this,
            &ARocketquakeRiffleWeapon::SmoothEnableFov,
            0.01f,
            true
        );
}

void ARocketquakeRiffleWeapon::SmoothEnableFov()
{
    const auto Controller = Cast<ARocketquakePlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager)
    {
        return;
    }

    const auto Character = Cast<ARocketQuakeCharacter>(Controller->GetCharacter());
    if (!Character) 
    {
        return;
    }

    if (FMath::IsNearlyEqual(Controller->PlayerCameraManager->GetFOVAngle(), DesiredCameraFov, 0.01f) &&
        FMath::IsNearlyEqual(Character->GetCameraLagSpeed(), DesiredCameraLagSpeed, 0.01f))
    {
        GetWorldTimerManager().ClearTimer(InZoomTimerHandle);
        return;
    }

    Controller->PlayerCameraManager->SetFOV(FMath::FInterpTo(Controller->PlayerCameraManager->GetFOVAngle(), DesiredCameraFov, GetWorld()->GetDeltaSeconds(), 3.0f));
    Character->CameraLagSpeed(FMath::FInterpTo(Character->GetCameraLagSpeed(), DesiredCameraLagSpeed, GetWorld()->GetDeltaSeconds(), 3.0f));
}

void ARocketquakeRiffleWeapon::Server_ForceStopShoot_Implementation()
{
    Multicast_ForceStopShoot();
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

    if (HitResult.bBlockingHit && HasAuthority())
    {
        TraceFxEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
    }

    WeaponFXComponent->PlayImpactFx(HitResult);
    
    Server_DecreaseAmmo();
    
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    if (Player->IsLocallyControlled())
    {
        PlayCameraShake();
    }
    
    SpawnTraceFx(TraceFxEnd);
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
        if (HitResult.GetActor())
        {
            UGameplayStatics::ApplyDamage(
                HitResult.GetActor(),
                FMath::RandRange(0.0f, Damage),
                GetController(),
                this,
                {});
        }
        return;
    }

    DamageActor->TakeDamage(FMath::RandRange(0.0f, Damage), FDamageEvent(), GetController(), this);
}

void ARocketquakeRiffleWeapon::ResetZoom()
{
    DesiredCameraFov = DefaultCameraFov;
    GetWorldTimerManager().ClearTimer(InZoomTimerHandle);
    
    const auto Controller = Cast<ARocketquakePlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager)
    {
        return;
    }
    
    const auto Character = Cast<ARocketQuakeCharacter>(Controller->GetCharacter());
    if (!Character) 
    {
        return;
    }
    
    Controller->PlayerCameraManager->SetFOV(DefaultCameraFov);
    Character->CameraLagSpeed(DefaultCameraLagSpeed);
    SetMouseZoomSensitivity(false);
}

void ARocketquakeRiffleWeapon::Server_StartShoot_Implementation()
{
    Multicast_StartShoot();
}

void ARocketquakeRiffleWeapon::Server_StopShoot_Implementation()
{
    Multicast_StopShoot();
}

void ARocketquakeRiffleWeapon::Local_StartShoot()
{
    if (IsAmmoEmpty())
    {
        return;
    }
    
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ARocketquakeRiffleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
    InitMuzzleFX_Implementation();
}

void ARocketquakeRiffleWeapon::Local_StopShoot()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);

    if (IsValidLowLevelFast())
    {
        SetMuzzleFXVisibility(false);
    }
}

void ARocketquakeRiffleWeapon::Multicast_StartShoot_Implementation()
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    if (Player->IsLocallyControlled())
    {
        return;
    }
    
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ARocketquakeRiffleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
    InitMuzzleFX_Implementation();
}

void ARocketquakeRiffleWeapon::Multicast_StopShoot_Implementation()
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    if (Player->IsLocallyControlled())
    {
        return;
    }
    
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);

    if (IsValidLowLevelFast())
    {
        SetMuzzleFXVisibility(false);
    }
}

void ARocketquakeRiffleWeapon::Multicast_ForceStopShoot_Implementation()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    if (IsValidLowLevelFast())
    {
        SetMuzzleFXVisibility(false);
    }
}
