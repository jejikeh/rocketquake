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
        // Client_DebugPrintAnimationsInformation();
        if (!CurrentWeapon)
        {
            UE_LOG(LogTemp, Error, TEXT("AAAA: Current Weapon is null"))
        }

        if (!CanShoot())
        {
            UE_LOG(LogTemp, Error, TEXT("AAAA: Can Shoot is false"))
            if (bIsEquippedInProgress)
            {
                UE_LOG(LogTemp, Error, TEXT("BBBB: is equipped in progress"))
            }

            if (bIsReloadInProgress)
            {
                UE_LOG(LogTemp, Error, TEXT("BBBB: is reload in progress"))
            }
        }
        
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

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("bIsEquippedInProgress: %s"), bIsEquippedInProgress ? TEXT("true") : TEXT("false"));

    InitAnimations();
    EquipWeapon(CurrentWeaponIndex);
    
    UE_LOG(LogTemp, Warning, TEXT("GetOwnerRole: %d"), GetOwnerRole())
    
    if (GetOwner()->HasAuthority())
    {
        SpawnWeapons();
        EquipWeapon(CurrentWeaponIndex);
    }

    UE_LOG(LogTemp, Warning, TEXT("Weapon Component Begin Play"))
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

    // if (!EquipAnimMontage)
    // {
    //     return;
    // }
    //
    // if (const auto EquipNotify = FindNotifyByClass<UEquipFinishedAnimNotify>(EquipAnimMontage))
    // {
    //     EquipNotify->OnNotified.RemoveAll(this);
    // }
    //
    // for (const auto WeaponData : WeaponClasses)
    // {
    //     auto ReloadNotify = FindNotifyByClass<UReloadAnimNotify>(WeaponData.ReloadAnimMontage);
    //     if (!ReloadNotify)
    //     {
    //         continue;
    //     }
    //
    //     ReloadNotify->OnNotified.RemoveAll(this);
    // }

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

void UWeaponComponent::Server_SetReloadAnimationInProgress_Implementation(bool InProgress)
{
    bIsReloadInProgress = InProgress;
}

void UWeaponComponent::Server_SetEquippedAnimationInProgress_Implementation(bool InProgress)
{
    UE_LOG(LogTemp, Warning, TEXT("Server_SetEquippedAnimationInProgress_Implementation: FIJEIOFJIO:EHGIULHEF"))
    bIsEquippedInProgress = InProgress;
}

void UWeaponComponent::Client_DebugPrintAnimationsInformation_Implementation()
{
    if (!CurrentWeapon)
    {
        UE_LOG(LogTemp, Error, TEXT("CLIENT_DEBUG: Current Weapon is null"))
    }
    
    UE_LOG(LogTemp, Error, TEXT("CLIENT_DEBUG: Can Shoot is false"))
    
    if (bIsEquippedInProgress)
    {
        UE_LOG(LogTemp, Error, TEXT("CLIENT_DEBUG: is equipped in progress"))
    }

    if (bIsReloadInProgress)
    {
        UE_LOG(LogTemp, Error, TEXT("CLIENT_DEBUG: is reload in progress"))
    }
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
    UE_LOG(LogTemp, Warning, TEXT("InitAnimations:"))
    
    if (!EquipAnimMontage)
    {
        return;
    }

    if (const auto EquipNotify = FindNotifyByClass<UEquipFinishedAnimNotify>(EquipAnimMontage))
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
    if (GetOwnerRole() != ROLE_AutonomousProxy && GetOwnerRole() != ROLE_Authority)
    {
        return;
    }
    
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    if (Character->GetMesh() == MeshComp)
    {
        bIsEquippedInProgress = false;
        Server_SetEquippedAnimationInProgress(false);
    }
}

void UWeaponComponent::OnReloadFinished(USkeletalMeshComponent *MeshComp)
{
    if (GetOwnerRole() != ROLE_AutonomousProxy && GetOwnerRole() != ROLE_Authority)
    {
        return;
    }
    
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    if (Character->GetMesh() == MeshComp)
    {
        bIsReloadInProgress = false;
        Server_SetReloadAnimationInProgress(false);
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

    // NOTE(jejikeh): Client doesnt actually stores AnimationProgress, he is only notifies Server. Is okay?
    bIsReloadInProgress = true;
    UE_LOG(LogTemp, Warning, TEXT("OGIEJIUGI: Set IsReloadInProgress"));
    Multicast_PlayAnimMontage(CurrentAnimMontage);
}
