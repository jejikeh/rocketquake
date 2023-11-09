// Rocketquake, jejikeh


#include "UI/PlayerHudWidget.h"

#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"

float UPlayerHudWidget::GetHealthPercent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return 0.0f;
    }

    const auto Component = Player->GetComponentByClass(UHealthComponent::StaticClass());
    const auto HealthComponent = Cast<UHealthComponent>(Component);

    if (!HealthComponent)
    {
        return 0.0f;
    }

    return HealthComponent->GetHealthPercent();
}

bool UPlayerHudWidget::GetWeaponUIData(FWeaponUIData& WeaponUIData) const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return false;
    }

    const auto Component = Player->GetComponentByClass(UWeaponComponent::StaticClass());
    const auto WeaponComponent = Cast<UWeaponComponent>(Component);

    if (!WeaponComponent)
    {
        return false;
    }
    
    return WeaponComponent->GetWeaponUIData(WeaponUIData);
}

bool UPlayerHudWidget::GetWeaponAmmoData(FAmmoData &AmmoData) const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return false;
    }

    const auto Component = Player->GetComponentByClass(UWeaponComponent::StaticClass());
    const auto WeaponComponent = Cast<UWeaponComponent>(Component);

    if (!WeaponComponent)
    {
        return false;
    }

    return WeaponComponent->GetWeaponAmmoData(AmmoData);
}

bool UPlayerHudWidget::IsPlayerAlive()
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return false;
    }

    const auto Component = Player->GetComponentByClass(UHealthComponent::StaticClass());
    const auto HealthComponent = Cast<UHealthComponent>(Component);

    if (!HealthComponent)
    {
        return false;
    }

    return !HealthComponent->IsDead();
}

bool UPlayerHudWidget::IsPlayerSpectating()
{
    const auto Controller = GetOwningPlayer();
    if (!Controller)
    {
        return false;
    }

    return Controller->GetStateName() == NAME_Spectating;
}

bool UPlayerHudWidget::Initialize()
{
    // const auto Player = GetOwningPlayerPawn();
    // if (!Player)
    // {
    //     return Super::Initialize();
    // }
    //
    // const auto Component = Player->GetComponentByClass(UHealthComponent::StaticClass());
    // const auto HealthComponent = Cast<UHealthComponent>(Component);
    // if (!HealthComponent)
    // {
    //     HealthComponent->OnHealthChanged.AddDynamic(this, &UPlayerHudWidget::OnHealthChanged);
    // }
    //
    return Super::Initialize();
}

void UPlayerHudWidget::OnHealthChanged()
{
    OnTakeDamage();
}
