// Rocketquake, jejikeh


#include "UI/GameDataWidget.h"

#include "RocketquakewGameStateBase.h"
#include "Player/RocketquakePlayerState.h"
#include "Rocketquake/RocketquakeGameModeBase.h"

int32 UGameDataWidget::GetKillsNum() const
{
    const auto PlayerState = GetPlayerState();
    if (!PlayerState)
    {
        return 0;
    }

    return PlayerState->GetKillsNum();
}

int32 UGameDataWidget::GetCurrentRoundNum() const
{
    const auto RocketquakewGameStateBase = GetGameState();
    return RocketquakewGameStateBase ? RocketquakewGameStateBase->GetCurrentRound() : 0;
}

int32 UGameDataWidget::GetTotalRoundsNum() const
{
    const auto RocketquakewGameStateBase = GetGameState();
    return RocketquakewGameStateBase ? RocketquakewGameStateBase->GetRounds() : 0;
}

int32 UGameDataWidget::GetRoundCountDown() const
{
    const auto RocketquakewGameStateBase = GetGameState();
    return RocketquakewGameStateBase ? RocketquakewGameStateBase->GetRoundCountDown() : 0;
}

int32 UGameDataWidget::GetDeathsCount() const
{
    const auto PlayerState = GetPlayerState();
    if (!PlayerState)
    {
        return 0;
    }

    return PlayerState->GetDeathNum();
}

ARocketquakeGameModeBase *UGameDataWidget::GetGameMode() const
{
    return Cast<ARocketquakeGameModeBase>(GetWorld()->GetAuthGameMode());
}

ARocketquakePlayerState *UGameDataWidget::GetPlayerState() const
{
    const auto Controller = GetOwningPlayer();
    if (!Controller)
    {
        return nullptr;
    }

    const auto PlayerState = Controller->PlayerState;
    if (!PlayerState)
    {
        return nullptr;
    }

    return Cast<ARocketquakePlayerState>(PlayerState);
}

ARocketquakewGameStateBase* UGameDataWidget::GetGameState() const
{
    return GetWorld()->GetGameState<ARocketquakewGameStateBase>();
}
