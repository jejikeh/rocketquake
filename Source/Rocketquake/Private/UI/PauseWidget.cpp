// Rocketquake, jejikeh


#include "UI/PauseWidget.h"

#include "Components/Button.h"
#include "Player/RocketquakePlayerState.h"
#include "Types/GameMatchStates.h"

class ARocketquakePlayerState;

bool UPauseWidget::Initialize()
{
    const auto InitStatus = Super::Initialize();

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UPauseWidget::OnPlayButtonClicked);
    }

    return InitStatus;
}

void UPauseWidget::OnPlayButtonClicked()
{
    if (const auto PlayerState = GetOwningPlayerState<ARocketquakePlayerState>())
    {
        PlayerState->SetCurrentGameState(EPlayerGameState::Playing);
    }
}
