// Rocketquake, jejikeh


#include "Components/WeaponComponent.h"

#include "GameFramework/Character.h"
#include "Weapon/RocketquakeWeapon.h"

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::StartShoot_Implementation()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->StartShoot();
}

void UWeaponComponent::StopShoot_Implementation()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->StopShoot();
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        SpawnWeapon();
    }
}

void UWeaponComponent::SpawnWeapon_Implementation()
{
    const auto Character = Cast<ACharacter>(GetOwner());

    if (!WeaponClass->IsValidLowLevelFast() || !Character)
    {
        return;
    }

    CurrentWeapon = GetWorld()->SpawnActor<ARocketquakeWeapon>(WeaponClass);
    CurrentWeapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
    CurrentWeapon->SetOwner(Character);
}
