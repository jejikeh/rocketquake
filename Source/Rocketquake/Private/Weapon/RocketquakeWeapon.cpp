// Rocketquake, jejikeh


#include "Weapon/RocketquakeWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Player/RocketQuakeCharacter.h"

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

void ARocketquakeWeapon::ForceStopShot()
{
}

void ARocketquakeWeapon::MakeShot()
{
}

void ARocketquakeWeapon::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmoData = AmmoDefaultData;
}

void ARocketquakeWeapon::MakeHit(FHitResult &HitResult, const FVector &Vector, const FVector &TraceEnd) const
{
    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    CollisionQueryParams.bReturnPhysicalMaterial = true;

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
        const auto Player = Cast<ARocketQuakeCharacter>(GetOwner());
        if (!Player)
        {
            return false;
        }
        
        ViewLocation = GetActorLocation();
        ViewRotation = Player->GetLastReplicatedViewRotation();
        
        return true;
    }

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}

void ARocketquakeWeapon::Server_DecreaseAmmo_Implementation()
{
    CurrentAmmoData.Bullets = FMath::Clamp(CurrentAmmoData.Bullets - 1, 0, AmmoDefaultData.Bullets);

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        ForceStopShot();
        OnClipEmpty.Broadcast();
    }

    if (IsAmmoEmpty())
    {
        ForceStopShot();
    }
}

bool ARocketquakeWeapon::IsAmmoEmpty()
{
    const auto IsOutOfBullets= !CurrentAmmoData.InfiniteAmmo && CurrentAmmoData.Clips <= 0 && IsClipEmpty();
    if (GetOwner()->HasAuthority())
    {
        bServerToldMeWhatImOutOfBullets = IsOutOfBullets;
    }
    else
    {
        return IsOutOfBullets;
    }

    return IsOutOfBullets;
}

bool ARocketquakeWeapon::IsClipEmpty() const
{
    return CurrentAmmoData.Bullets <= 0;
}

void ARocketquakeWeapon::ChangeClip_Implementation()
{
    if (!CurrentAmmoData.InfiniteAmmo)
    {
        CurrentAmmoData.Clips--;
    }

    CurrentAmmoData.Bullets = AmmoDefaultData.Bullets;
}

bool ARocketquakeWeapon::CanReload() const
{
    return CurrentAmmoData.Bullets <= AmmoDefaultData.Bullets && CurrentAmmoData.Clips > 0;
}

UNiagaraComponent* ARocketquakeWeapon::SpawnMuzzleFlash() const
{
    return UNiagaraFunctionLibrary::SpawnSystemAttached(
        NiagaraSystem,
        WeaponMesh,
        TEXT("MuzzleFlashSocket"),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true);
}

bool ARocketquakeWeapon::AddAmmo(int32 AmmoCount)
{
    if (CurrentAmmoData.InfiniteAmmo || AmmoCount <= 0)
    {
        return false;
    }

    if (CurrentAmmoData.Clips == AmmoDefaultData.Clips && CurrentAmmoData.Bullets == AmmoDefaultData.Bullets)
    {
        return false;
    }

    if (IsAmmoEmpty())
    {
        CurrentAmmoData.Clips = FMath::Clamp(CurrentAmmoData.Clips + AmmoCount, 0, AmmoDefaultData.Clips + 1);
        OnClipEmpty.Broadcast();
    }

    else if (CurrentAmmoData.Clips < AmmoDefaultData.Clips)
    {
        const auto NextClipsAmount = CurrentAmmoData.Clips + AmmoCount;
        if (AmmoDefaultData.Clips - NextClipsAmount >= 0)
        {
            CurrentAmmoData.Clips = NextClipsAmount;
        }
        else
        {
            CurrentAmmoData.Clips = AmmoDefaultData.Clips;
            CurrentAmmoData.Bullets = AmmoDefaultData.Bullets;
        }
    }
    else
    {
        CurrentAmmoData.Bullets = AmmoDefaultData.Bullets;
    }

    return true;
}

void ARocketquakeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ARocketquakeWeapon, bServerToldMeWhatImOutOfBullets);
}

