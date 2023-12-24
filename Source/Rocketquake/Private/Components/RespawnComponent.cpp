// Rocketquake, jejikeh


#include "Components/RespawnComponent.h"
#include "Rocketquake/RocketquakeGameModeBase.h"

URespawnComponent::URespawnComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URespawnComponent::Client_Respawn_Implementation(const int RespawnTime)
{
    RespawnCountDown = RespawnTime;
    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::RespawnTimerUpdate, 1.0f, true);
}

void URespawnComponent::Client_ResetRespawnTimer_Implementation()
{
    RespawnCountDown = 0;
    GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
}

bool URespawnComponent::IsRespawnInProgress() const
{
    return GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle);
}

void URespawnComponent::RespawnTimerUpdate()
{
    RespawnCountDown--;

    if (RespawnCountDown <= 0)
    {
        RespawnCountDown = 0;
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

        const auto GameMode = Cast<ARocketquakeGameModeBase>(GetWorld()->GetAuthGameMode());
        if (!GameMode)
        {
            return;
        }

        GameMode->RespawnRequest(Cast<AController>(GetOwner()));
    }
}
