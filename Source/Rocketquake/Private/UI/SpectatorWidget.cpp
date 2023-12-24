// Rocketquake, jejikeh


#include "UI/SpectatorWidget.h"

#include "Components/RespawnComponent.h"

bool USpectatorWidget::GetRespawnTime(int &RespawnTime) const
{
    const auto RespawnComponent = GetOwningPlayer()->FindComponentByClass<URespawnComponent>();
    if (!RespawnComponent || !RespawnComponent->IsRespawnInProgress())
    {
        return false;
    }

    RespawnTime = RespawnComponent->GetRespawnCountDown();
    return true;
}
