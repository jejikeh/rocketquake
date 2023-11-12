// Rocketquake, jejikeh


#include "UI/RocketquakeHUD.h"

#include "RocketquakewGameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "Player/RocketquakePlayerState.h"


void ARocketquakeHUD::DrawHUD()
{
    Super::DrawHUD();
}

void ARocketquakeHUD::BeginPlay()
{
    Super::BeginPlay();
    
    GameWidgets.Add(EPlayerGameState::Playing, CreateWidget<UUserWidget>(GetWorld(), PlayerHudWidgetClass));
    GameWidgets.Add(EPlayerGameState::Paused, CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(EPlayerGameState::GameOver, CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass));

    for (auto &Widget : GameWidgets)
    {
        const auto GameWidget = Widget.Value;
        if (!GameWidget)
        {
            continue;
        }
        
        GameWidget->AddToViewport();
        GameWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    GetWorldTimerManager().SetTimer(WaitPlayerStateTimer, this, &ARocketquakeHUD::WaitPlayerState, 0.1f, true);
}

void ARocketquakeHUD::OnPlayerGameStateChanged(EPlayerGameState NewState)
{
    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (GameWidgets.Contains(NewState))
    {
        CurrentWidget = GameWidgets[NewState];
    }

    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void ARocketquakeHUD::WaitPlayerState()
{
    if (const auto PlayerState = GetOwningPlayerController()->GetPlayerState<ARocketquakePlayerState>())
    {
        UE_LOG(LogTemp, Error, TEXT("HOORAAAy"));
        
        PlayerState->OnPlayerGameStateChanged.AddDynamic(this, &ARocketquakeHUD::OnPlayerGameStateChanged);
        if (const auto ProperToStateWidget = GameWidgets[PlayerState->GetCurrentGameState()])
        {
            ProperToStateWidget->SetVisibility(ESlateVisibility::Visible);
        }

        GetWorldTimerManager().ClearTimer(WaitPlayerStateTimer);
    }
}
