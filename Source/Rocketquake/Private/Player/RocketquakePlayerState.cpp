// Rocketquake, jejikeh


#include "Player/RocketquakePlayerState.h"

#include "RocketquakewGameStateBase.h"
#include "Net/UnrealNetwork.h"

class ARocketquakewGameStateBase;

void ARocketquakePlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (const auto GameState = GetWorld()->GetGameState<ARocketquakewGameStateBase>())
    {
        GameState->OnMatchStateChanged.AddDynamic(this, &ARocketquakePlayerState::OnMatchStateChanged);
    }
}

void ARocketquakePlayerState::OnMatchStateChanged(ERocketquakeMatchState NewState)
{
    if (NewState == ERocketquakeMatchState::InProgress)
    {
        SetCurrentGameState(EPlayerGameState::Playing);
    }

    if (NewState == ERocketquakeMatchState::GameOver)
    {
        SetCurrentGameState(EPlayerGameState::GameOver);
    }
}

void ARocketquakePlayerState::AddKill(const FString& Victim)
{
    KillsNum++;
    Multicast_BroadcastOnKillPlayer(Victim);
}

void ARocketquakePlayerState::SetCurrentGameState(EPlayerGameState NewState)
{
    CurrentGameState = NewState;

    OnPlayerGameStateChanged.Broadcast(CurrentGameState);
}

void ARocketquakePlayerState::Multicast_BroadcastOnKillPlayer_Implementation(const FString& Victim)
{
    // OnKillPlayer.Broadcast(Victim);
}

void ARocketquakePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARocketquakePlayerState, TeamID);
    DOREPLIFETIME(ARocketquakePlayerState, KillsNum);
    DOREPLIFETIME(ARocketquakePlayerState, DeathNus);
    DOREPLIFETIME(ARocketquakePlayerState, TeamColor);
}
