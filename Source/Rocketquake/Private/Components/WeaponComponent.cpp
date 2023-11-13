// Rocketquake, jejikeh


#include "Components/WeaponComponent.h"

#include "Animations/EquipFinishedAnimNotify.h"
#include "Animations/ReloadAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/RocketquakeWeapon.h"

class UReloadAnimNotify;

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::StartShoot_Implementation()
{
    if (!CurrentWeapon || !CanShoot())
    {
        return;
    }

    CurrentWeapon->StartShoot();
}

// TODO: Make shoot on client local

void UWeaponComponent::StopShoot_Implementation()
{
    if (!CurrentWeapon || !CurrentWeapon->IsValidLowLevelFast())
    {
        return;
    }

    CurrentWeapon->StopShoot();
}

void UWeaponComponent::NextWeapon_Implementation()
{
    if (!CanEquip())
    {
        return;
    }

    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}

void UWeaponComponent::Reload_Implementation()
{
    ChangeClip();
}

bool UWeaponComponent::GetWeaponUIData(FWeaponUIData &WeaponUIData) const
{
    if (GetOwner()->HasAuthority())
    {
        if (!CurrentWeapon)
        {
            return false;
        }

        WeaponUIData = CurrentWeapon->GetWeaponUIData();
        return true;
    }

    if (!CurrentWeaponUIData.Crosshair || !CurrentWeaponUIData.Icon)
    {
        WeaponUIData = DefaultWeaponUIData;
        return true;
    }

    WeaponUIData = CurrentWeaponUIData;
    return true;
}

bool UWeaponComponent::GetWeaponAmmoData(FAmmoData &AmmoData)
{
    if (GetOwner()->HasAuthority())
    {
        if (!CurrentWeapon)
        {
            return false;
        }

        AmmoData = CurrentWeapon->GetAmmoData();
        
        return true;
    }

    Server_SetClientWeaponAmmoData();
    AmmoData = CurrentAmmoData;
    return true;
}

bool UWeaponComponent::TryToAddAmmo(TSubclassOf<ARocketquakeWeapon> Class, int32 AmmoCount)
{
    if (!CurrentWeapon)
    {
        return false;
    }

    if (CurrentWeapon->IsA(Class))
    {
        return CurrentWeapon->AddAmmo(AmmoCount);
    }

    return false;
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        SpawnWeapons();
        EquipWeapon(CurrentWeaponIndex);
    }

    InitAnimations();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (const auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }

    Weapons.Empty();

    Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::SpawnWeapons_Implementation()
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    for (const auto WeaponClass : WeaponClasses)
    {
        const auto Weapon = GetWorld()->SpawnActor<ARocketquakeWeapon>(WeaponClass.WeaponClasses);
        if (!Weapon)
        {
            continue;
        }

        Weapon->OnClipEmpty.AddUObject(this, &UWeaponComponent::OnEmptyClip);
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);
        Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("ArmorySocket"));
    }
}

void UWeaponComponent::EquipWeapon_Implementation(int32 Index)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character || Weapons.IsEmpty())
    {
        return;
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->StopShoot();
        CurrentWeapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("ArmorySocket"));
    }

    if (Index < 0 || Index >= Weapons.Num())
    {
        Index = 0;
    }

    CurrentWeapon = Weapons[Index];

    const auto CurrentWeaponData = WeaponClasses.FindByPredicate([&](const FWeaponData &Weapon)
    {
        return Weapon.WeaponClasses == CurrentWeapon->GetClass();
    });

    CurrentAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
    CurrentWeapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
    Multicast_PlayAnimMontage(EquipAnimMontage);
    bIsEquippedInProgress = true;

    Client_SetCurrentWeaponUIData(CurrentWeapon->GetWeaponUIData());
}

void UWeaponComponent::Multicast_PlayAnimMontage_Implementation(UAnimMontage *AnimMontage)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    Character->PlayAnimMontage(AnimMontage);
}

void UWeaponComponent::Client_SetCurrentWeaponUIData_Implementation(FWeaponUIData WeaponUIData)
{
    CurrentWeaponUIData = WeaponUIData;
}

void UWeaponComponent::Server_SetClientWeaponAmmoData_Implementation()
{
    Client_SetCurrentWeaponAmmoData(CurrentWeapon->GetAmmoData());
}

void UWeaponComponent::Client_SetCurrentWeaponAmmoData_Implementation(FAmmoData AmmoData)
{
    CurrentAmmoData = AmmoData;
}

void UWeaponComponent::InitAnimations()
{
    if (!EquipAnimMontage)
    {
        return;
    }

    auto EquipNotify = FindNotifyByClass<UEquipFinishedAnimNotify>(EquipAnimMontage);
    if (EquipNotify)
    {
        EquipNotify->OnNotified.AddUObject(this, &UWeaponComponent::OnEquipFinished);
    }

    for (const auto WeaponData : WeaponClasses)
    {
        auto ReloadNotify = FindNotifyByClass<UReloadAnimNotify>(WeaponData.ReloadAnimMontage);
        if (!ReloadNotify)
        {
            continue;
        }

        ReloadNotify->OnNotified.AddUObject(this, &UWeaponComponent::OnReloadFinished);
    }
}

void UWeaponComponent::OnEquipFinished(USkeletalMeshComponent *MeshComp)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    if (Character->GetMesh() == MeshComp)
    {
        bIsEquippedInProgress = false;
    }
}

void UWeaponComponent::OnReloadFinished(USkeletalMeshComponent *MeshComp)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    if (Character->GetMesh() == MeshComp)
    {
        bIsReloadInProgress = false;
    }
}

bool UWeaponComponent::CanEquip() const
{
    return !bIsEquippedInProgress && !bIsReloadInProgress;
}

bool UWeaponComponent::CanShoot() const
{
    return CurrentWeapon && !bIsEquippedInProgress && !bIsReloadInProgress;
}

bool UWeaponComponent::CanReload() const
{
    return CurrentWeapon && !bIsEquippedInProgress && !bIsReloadInProgress && CurrentWeapon->CanReload();
}

void UWeaponComponent::OnEmptyClip()
{
    ChangeClip();
}

void UWeaponComponent::ChangeClip()
{
    if (!CanReload())
    {
        return;
    }

    CurrentWeapon->StopShoot();
    CurrentWeapon->ChangeClip();

    bIsReloadInProgress = true;
    Multicast_PlayAnimMontage(CurrentAnimMontage);
}
