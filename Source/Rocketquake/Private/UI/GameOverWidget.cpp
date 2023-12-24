// Rocketquake, jejikeh


#include "UI/GameOverWidget.h"

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RocketquakePlayerState.h"
#include "UI/PlayerStatWidget.h"

class ARocketquakePlayerState;

bool UGameOverWidget::Initialize()
{
    const auto InitStatus = Super::Initialize();
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(WaitPlayerStateTimer, this, &UGameOverWidget::WaitPlayerState, 0.1f, true);
    }

    if (RestartButton)
    {
        UE_LOG(LogTemp, Error, TEXT("ERROR IM ERROR"));
        RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartButtonClicked);
    }
    
    return InitStatus;
}

void UGameOverWidget::OnPlayerGameStateChanged(EPlayerGameState NewState)
{
    if (NewState == EPlayerGameState::GameOver)
    {
        UpdatePlayerStats();
    }
}

void UGameOverWidget::UpdatePlayerStats()
{
    PlayerStatBox->ClearChildren();
    
    for (auto It = GetWorld()->GetGameState()->PlayerArray.CreateIterator(); It; ++It)
    {
        const auto PlayerStateUnwrapped = It->Get();
        if (!PlayerStateUnwrapped || !PlayerStateUnwrapped->IsA(ARocketquakePlayerState::StaticClass()))
        {
            continue;
        }

        const auto PlayerState = Cast<ARocketquakePlayerState>(PlayerStateUnwrapped);
        if (!PlayerState)
        {
            return;
        }

        const auto PlayerStatWidget = CreateWidget<UPlayerStatWidget>(GetWorld(), PlayerStatWidgetClass);
        if (!PlayerStatWidget)
        {
            return;
        }

        PlayerStatWidget->SetPlayerName(PlayerState->GetPlayerName());
        PlayerStatWidget->SetScore(PlayerState->GetTeamID());
        PlayerStatWidget->SetDeaths(PlayerState->GetDeathNum());
        PlayerStatWidget->SetKills(PlayerState->GetKillsNum());
        PlayerStatWidget->SetPlayerIndicatorVisible(PlayerState->HasLocalNetOwner());

        PlayerStatBox->AddChild(PlayerStatWidget);
    }
}

void UGameOverWidget::WaitPlayerState()
{
    if (!GetWorld() || !GetWorld()->GetFirstPlayerController())
    {
        return;
    }
    
    if (const auto PlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ARocketquakePlayerState>())
    {
        PlayerState->OnPlayerGameStateChanged.AddDynamic(this, &UGameOverWidget::OnPlayerGameStateChanged);
        GetWorld()->GetTimerManager().ClearTimer(WaitPlayerStateTimer);
    }
}

void UGameOverWidget::OnRestartButtonClicked()
{
    const auto CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
    GetWorld()->ServerTravel(CurrentLevelName, false);
}
