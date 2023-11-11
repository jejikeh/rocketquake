// Rocketquake, jejikeh


#include "Components/RespawnComponent.h"
#include "Rocketquake/RocketquakeGameModeBase.h"

URespawnComponent::URespawnComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URespawnComponent::Respawn(const int RespawnTime)
{
    RespawnCountDown = RespawnTime;
    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::RespawnTimerUpdate, 1.0f, true);
}

void URespawnComponent::RespawnTimerUpdate()
{
    RespawnCountDown--;

    UE_LOG(LogTemp, Warning, TEXT("RespawnCountDown: %d"), RespawnCountDown);
    
    if (RespawnCountDown <= 0)
    {
        RespawnCountDown = 0;
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

        const auto GameMode = Cast<ARocketquakeGameModeBase>(GetWorld()->GetAuthGameMode());
        if (!GameMode)
        {
            UE_LOG(LogTemp, Error, TEXT("GameMode is null"));
            return;
        }

        GameMode->RespawnRequest(Cast<AController>(GetOwner()));
    }
}
