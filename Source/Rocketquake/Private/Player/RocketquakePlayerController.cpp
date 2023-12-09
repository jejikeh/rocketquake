// Rocketquake, jejikeh

#include "Player/RocketquakePlayerController.h"

#include "EnhancedInputComponent.h"
#include "Components/RespawnComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "Player/RocketquakePlayerState.h"


ARocketquakePlayerController::ARocketquakePlayerController()
{
    RespawnComponent = CreateDefaultSubobject<URespawnComponent>("RespawnComponent");
}

void ARocketquakePlayerController::StartSpectating()
{
    if (!HasAuthority())
    {
        return;
    }

    ChangeState(NAME_Spectating);
    Client_StartSpectating();
}

void ARocketquakePlayerController::StartPlaying()
{
    if (!HasAuthority())
    {
        SetInputMode(FInputModeGameOnly());
        return;
    }

    ChangeState(NAME_Playing);
    ClientGotoState(NAME_Playing);
    Client_SetInputMode();
}

void ARocketquakePlayerController::Client_StartSpectating_Implementation()
{
    if (PlayerCameraManager)
    {
        SetSpawnLocation(PlayerCameraManager->GetCameraLocation());
    }

    ChangeState(NAME_Spectating);
}

void ARocketquakePlayerController::OnRep_Pawn()
{
    Super::OnRep_Pawn();

    if (GetStateName() == NAME_Spectating)
    {
        AutoManageActiveCameraTarget(GetSpectatorPawn());
    }
}

void ARocketquakePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ARocketquakePlayerController::OnPauseGame);
    }
}

void ARocketquakePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (const auto RocketquakePlayerState = GetPlayerState<ARocketquakePlayerState>())
    {
        RocketquakePlayerState->OnPlayerGameStateChanged.AddDynamic(this, &ARocketquakePlayerController::OnPlayerStateChanged);
    } else
    {
        GetWorldTimerManager().SetTimer(WaitPlayerStateTimer, this, &ARocketquakePlayerController::WaitPlayerState, 1.0f, true);
    }
}

void ARocketquakePlayerController::OnPauseGame()
{
    if (const auto RocketquakePlayerState = GetPlayerState<ARocketquakePlayerState>())
    {
        if (RocketquakePlayerState->GetCurrentGameState() != EPlayerGameState::Paused && RocketquakePlayerState->GetCurrentGameState() != EPlayerGameState::GameOver)
        {
            RocketquakePlayerState->SetCurrentGameState(EPlayerGameState::Paused);
        }
    }
}

void ARocketquakePlayerController::OnPlayerStateChanged(EPlayerGameState PlayerGameState)
{
    if (PlayerGameState == EPlayerGameState::Playing)
    {
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly{});
    }
    else
    {
        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly{});
    }
}

void ARocketquakePlayerController::WaitPlayerState()
{
    if (const auto RocketquakePlayerState = GetPlayerState<ARocketquakePlayerState>())
    {
        RocketquakePlayerState->OnPlayerGameStateChanged.AddDynamic(this, &ARocketquakePlayerController::OnPlayerStateChanged);
        GetWorldTimerManager().ClearTimer(WaitPlayerStateTimer);
    }
}

void ARocketquakePlayerController::Client_SetInputMode_Implementation()
{
    SetInputMode(FInputModeGameOnly());
}
