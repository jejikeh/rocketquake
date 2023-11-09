// Rocketquake, jejikeh

#include "Player/RocketquakePlayerController.h"
#include "GameFramework/SpectatorPawn.h"


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
        return;
    }

    ChangeState(NAME_Playing);
    ClientGotoState(NAME_Playing);
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
