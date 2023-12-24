// Rocketquake, jejikeh


#include "RocketquakewGameStateBase.h"

#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Rocketquake/RocketquakeGameModeBase.h"

void ARocketquakewGameStateBase::UpdateRoundCountDown(const int32 NewRoundCountDown)
{
    if (HasAuthority())
    {
        RoundCountDown = NewRoundCountDown;
    }
}

void ARocketquakewGameStateBase::UpdateCurrentRound(const int32 NewCurrentRound)
{
    if (HasAuthority())
    {
        CurrentRound = NewCurrentRound;
    }
}

void ARocketquakewGameStateBase::SetRounds(const int32 NewRounds)
{
    if (HasAuthority())
    {
        Rounds = NewRounds;
    }
}

void ARocketquakewGameStateBase::SetCurrentMatchState(ERocketquakeMatchState NewState)
{
    if (HasAuthority())
    {
        CurrentMatchState = NewState;
        OnMatchStateChanged.Broadcast(CurrentMatchState);
    }
}

void ARocketquakewGameStateBase::Multicast_EndGame_Implementation()
{
    for (const auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn) 
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }
}

void ARocketquakewGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARocketquakewGameStateBase, RoundCountDown);
    DOREPLIFETIME(ARocketquakewGameStateBase, CurrentRound);
    DOREPLIFETIME(ARocketquakewGameStateBase, Rounds);
    DOREPLIFETIME(ARocketquakewGameStateBase, CurrentMatchState);
}

void ARocketquakewGameStateBase::OnRep_CurrentMatchState()
{
    OnMatchStateChanged.Broadcast(CurrentMatchState);
}
