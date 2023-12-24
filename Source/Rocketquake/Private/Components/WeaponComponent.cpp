// Rocketquake, jejikeh


#include "Components/WeaponComponent.h"

#include "Animations/EquipFinishedAnimNotify.h"
#include "Animations/ReloadAnimNotify.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/RocketquakeWeapon.h"

class UReloadAnimNotify;

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }

    InitAnimations();
    // NOTE(jejikeh): Check if that works, then clean UFUNCTION macro
    SpawnWeapons_Implementation();
    EquipWeapon(CurrentWeaponIndex);

    if (Player->IsLocallyControlled())
    {
        Player->PlayAnimMontage(EquipAnimMontage);
        Client_SetCurrentWeaponUIData_Implementation(Weapons[CurrentWeaponIndex]->GetWeaponUIData());
    }
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

    // NOTE(jejikeh): Maybe should create another method to play animations on the client
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    Character->PlayAnimMontage(EquipAnimMontage);
    bIsEquippedInProgress = true;

    auto NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

    if (CurrentWeapon)
    {
        CurrentWeapon->ResetZoom();
    }
    
    EquipWeapon(NextWeaponIndex);

    // HACK(jejikeh): If response from the server is not received yet, then we manually set the current weapon ui data
    if (NextWeaponIndex == (CurrentWeaponIndex + 1) % Weapons.Num())
    {
        if (NextWeaponIndex < 0 || NextWeaponIndex >= Weapons.Num())
        {
            NextWeaponIndex = 0;
        }
        
        Client_SetCurrentWeaponUIData_Implementation(Weapons[NextWeaponIndex]->GetWeaponUIData());
    }
}

void UWeaponComponent::Multicast_ResetAnimNotifies_Implementation()
{
    bIsEquippedInProgress = false;
    bIsReloadInProgress = false;
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

void UWeaponComponent::Zoom(bool Enabled)
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->Zoom(Enabled);
}

void UWeaponComponent::ResetZoom()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->ResetZoom();
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

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // NOTE(jejikeh): This is a hack. For some reason without it the bIsEquippedInProgress and bIsReloadInProgress are always false
    // On initial spawned weapon
    if (bIsFirstTick)
    {
        const auto Character = Cast<ACharacter>(GetOwner());
        if (!Character)
        {
            return;
        }

        if (!Character->IsLocallyControlled())
        {
            bIsFirstTick = false;
            return;
        }
        
        NextWeapon();
        bIsFirstTick = false;
    }
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWeaponComponent, CurrentWeaponIndex);
    DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
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
    CurrentWeaponIndex = Index;
    
    const auto CurrentWeaponData = WeaponClasses.FindByPredicate([&](const FWeaponData &Weapon)
    {
        return Weapon.WeaponClasses == Weapons[CurrentWeaponIndex]->GetClass();
    });

    CurrentAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
    CurrentWeapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

    MulticastSkipOwner_PlayEquipAnimMontage();
    // Client_SetCurrentWeaponUIData(CurrentWeapon->GetWeaponUIData());
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

void UWeaponComponent::Server_PlayReloadAnimMontage_Implementation()
{
    Multicast_PlayReloadAnimMontage();
}

void UWeaponComponent::Multicast_PlayReloadAnimMontage_Implementation()
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    bIsReloadInProgress = true;
    Character->PlayAnimMontage(CurrentAnimMontage);
}

void UWeaponComponent::MulticastSkipOwner_PlayEquipAnimMontage_Implementation()
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    // NOTE(jejikeh): equip animation plays on the client in NextWeapon method.
    if (Character->IsLocallyControlled())
    {
        return;
    }

    Character->PlayAnimMontage(EquipAnimMontage);
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
        UE_LOG(LogTemp, Warning, TEXT("OnReloadFinished:"))
        bIsReloadInProgress = false;
        Server_SetReloadAnimationInProgress(false);
    }
}

void UWeaponComponent::OnRep_CurrentWeaponIndex()
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    // NOTE(jejikeh): May not be needed, but just in case
    if (Character->IsLocallyControlled())
    {
        Client_SetCurrentWeaponUIData_Implementation(Weapons[CurrentWeaponIndex]->GetWeaponUIData());
    }

    if (Weapons.IsEmpty())
    {
        return;
    }

    const auto CurrentWeaponData = WeaponClasses.FindByPredicate([&](const FWeaponData &Weapon)
    {
        return Weapon.WeaponClasses == Weapons[CurrentWeaponIndex]->GetClass();
    });

    CurrentAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
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

void UWeaponComponent::Reload_Implementation()
{
    ChangeClip();
}

void UWeaponComponent::OnEmptyClip()
{
    UE_LOG(LogTemp, Warning, TEXT("OnEmptyClip:"));
    ChangeClip();
}

void UWeaponComponent::ChangeClip()
{
    if (!CanReload())
    {
        return;
    }

    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("ChangeClip"));
    }

    CurrentWeapon->StopShoot();
    CurrentWeapon->ChangeClip();

    // NOTE(jejikeh): Call from server
    Server_PlayReloadAnimMontage();
}

