// Rocketquake, jejikeh


#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Rocketquake/RocketquakeGameModeBase.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UHealthComponent::AddHealth(float HealthAmount)
{
    if (FMath::IsNearlyEqual(Health, MaxHealth) || IsDead())
    {
        return false;
    }

    Health = FMath::Min(Health + HealthAmount, MaxHealth);
    OnHealthChanged.Broadcast();
    
    return true;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;
    OnHealthChanged.Broadcast();

    if (const auto Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamageHandle);
    }
}

void UHealthComponent::OnTakeAnyDamageHandle(AActor *DamagedActor, float Damage, const UDamageType *DamageType,
    AController *InstigatedBy, AActor *DamageCauser)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld())
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Health: %f"), Health);
    
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast();

    if (IsDead())
    {
        Killed(InstigatedBy);
        UE_LOG(LogTemp, Display, TEXT("Killed, Health: %f"), Health);
        OnDeath.Broadcast();
    }
    else if (bAutoHeal)
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &UHealthComponent::HealthHealUpdate, HealSpeedRate, true, HealDelay);
    }

    Client_PlayCameraShake();
}

void UHealthComponent::HealthHealUpdate()
{
    if (IsDead())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
    
    Health = FMath::Min(Health + HealModifier, MaxHealth);
    OnHealthChanged.Broadcast();

    if (FMath::IsNearlyEqual(Health, MaxHealth) && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void UHealthComponent::Client_PlayCameraShake_Implementation()
{
    if (IsDead())
    {
        return;
    }

    const auto Player = Cast<APawn>(GetOwner());
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

void UHealthComponent::Killed(AController *Killer)
{
    if (!bIsTrackShooter)
    {
        return;
    }
        
    const auto GameMode = Cast<ARocketquakeGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        return;
    }

    const auto Player = Cast<APawn>(GetOwner());
    const auto Victim = Player ? Player->Controller : nullptr;

    GameMode->Killed(Killer, Victim);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, Health);
}
