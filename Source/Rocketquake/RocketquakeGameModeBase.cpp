// Copyright Epic Games, Inc. All Rights Reserved.


#include "RocketquakeGameModeBase.h"

#include "EngineUtils.h"
#include "RocketquakewGameStateBase.h"
#include "Components/RespawnComponent.h"
#include "Player/RocketQuakeCharacter.h"
#include "Player/RocketquakePlayerController.h"
#include "Player/RocketquakePlayerState.h"
#include "UI/RocketquakeHUD.h"

ARocketquakeGameModeBase::ARocketquakeGameModeBase()
{
    DefaultPawnClass = ARocketQuakeCharacter::StaticClass();
    PlayerControllerClass = ARocketquakePlayerController::StaticClass();
    HUDClass = ARocketquakeHUD::StaticClass();
    PlayerStateClass = ARocketquakePlayerState::StaticClass();
    GameStateClass = ARocketquakewGameStateBase::StaticClass();
}

void ARocketquakeGameModeBase::StartPlay()
{
    Super::StartPlay();

    if (const auto GameStateBase = GetGameState<ARocketquakewGameStateBase>())
    {
        GameStateBase->SetRounds(GameData.Rounds);
    }

    CurrentRound = 1;
    StartRound();
    CreateTeamsInfo();

    SetMatchState(ERocketquakeMatchState::InProgress);
}

void ARocketquakeGameModeBase::Killed(AController *Killer, AController *Victim)
{
    const auto KilledPlayerState = Killer ? Cast<ARocketquakePlayerState>(Killer->PlayerState) : nullptr;
    const auto VictimPlayerState = Victim ? Cast<ARocketquakePlayerState>(Victim->PlayerState) : nullptr;

    if (KilledPlayerState)
    {
        KilledPlayerState->AddKill(Victim->GetName());
    }

    if (VictimPlayerState)
    {
        VictimPlayerState->AddDeath();
    }

    if (Victim)
    {
        StartRespawn(Victim);
    }
}

void ARocketquakeGameModeBase::RestartGame()
{
    ResetPlayers();
    StartRound();
}

void ARocketquakeGameModeBase::GenericPlayerInitialization(AController *C)
{
    Super::GenericPlayerInitialization(C);

    ResetPlayers();
    StartRound();
    CreateTeamsInfo();
}

void ARocketquakeGameModeBase::StartRound()
{
    RoundCountDown = GameData.RoundTimeSecs;

    if (const auto GameStateBase = GetGameState<ARocketquakewGameStateBase>())
    {
        GameStateBase->UpdateCurrentRound(CurrentRound);
        GameStateBase->UpdateRoundCountDown(GameData.RoundTimeSecs);
    }
    
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ARocketquakeGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ARocketquakeGameModeBase::GameTimerUpdate()
{
    RoundCountDown--;

    if (const auto GameStateBase = GetGameState<ARocketquakewGameStateBase>())
    {
        GameStateBase->UpdateRoundCountDown(RoundCountDown);
    }
    
    if (RoundCountDown <= 0)
    {
        // TODO: Delete that? Move to event tick?
        GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);

        if (CurrentRound < GameData.Rounds)
        {
            CurrentRound++;
            ResetPlayers();
            StartRound();
            OnMatchChanged.Broadcast();
        }
        else
        {
            GameOver();
        }
    }
}

void ARocketquakeGameModeBase::ResetPlayers()
{
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ResetOnePlayer(It->Get());
        if (const auto RespawnComponent = It->Get()->GetComponentByClass<URespawnComponent>())
        {
            RespawnComponent->Client_ResetRespawnTimer();
        }
    }
}

void ARocketquakeGameModeBase::ResetOnePlayer(AController *Controller)
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();
    }

    RestartPlayer(Controller);
    SetPlayerColor(Controller);
}

void ARocketquakeGameModeBase::CreateTeamsInfo()
{
    int32 TeamID = 1;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        UE_LOG(LogTemp, Warning, TEXT("TeamID: %d"), TeamID);
        const auto Controller = It->Get();
        if (!Controller)
        {
            continue;
        }

        const auto PlayerState = Cast<ARocketquakePlayerState>(Controller->PlayerState);
        if (!PlayerState)
        {
            continue;
        }

        PlayerState->SetTeamID(TeamID);
        PlayerState->SetTeamColor(GetColorByTeamId(TeamID));
        PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot");
        SetPlayerColor(Controller);

        TeamID %= GameData.Players + 1;
        ++TeamID;
    }
}

FLinearColor ARocketquakeGameModeBase::GetColorByTeamId(int32 TeamId) const
{
    if (TeamId - 1 < GameData.TeamColors.Num())
    {
        return GameData.TeamColors[TeamId - 1];
    }

    return GameData.DefaultTeamColor;
}

void ARocketquakeGameModeBase::SetPlayerColor(AController *Controller)
{
    if (!Controller)
    {
        return;
    }

    const auto Character  = Cast<ARocketQuakeCharacter>(Controller->GetPawn());
    if (!Character)
    {
        return;
    }

    const auto PlayerState = Cast<ARocketquakePlayerState>(Controller->PlayerState);
    if (!PlayerState)
    {
        return;
    }

    Character->SetPlayerColor(PlayerState->GetTeamColor());
}

void ARocketquakeGameModeBase::RespawnRequest(AController *Controller)
{
    ResetOnePlayer(Controller);
}

void ARocketquakeGameModeBase::StartRespawn(const AController *Controller) const
{
    const auto RespawnComponent = Controller->FindComponentByClass<URespawnComponent>();
    if (!RespawnComponent)
    {
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("StartRespawn"));
    RespawnComponent->Client_Respawn(GameData.RespawnTime);
    if (HasAuthority())
    {
        RespawnComponent->Client_Respawn_Implementation(GameData.RespawnTime);
    }
}

void ARocketquakeGameModeBase::GameOver()
{
    if (const auto GameStateBase = GetGameState<ARocketquakewGameStateBase>())
    {
        GameStateBase->Multicast_EndGame();
    }

    SetMatchState(ERocketquakeMatchState::GameOver);
}


void ARocketquakeGameModeBase::SetMatchState(ERocketquakeMatchState NewState)
{
    if (CurrentMatchState == NewState)
    {
        return;
    }

    CurrentMatchState = NewState;
    
    if (const auto GameStateBase = GetGameState<ARocketquakewGameStateBase>())
    {
        GameStateBase->SetCurrentMatchState(NewState);
    }
}
