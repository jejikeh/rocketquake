// Rocketquake, jejikeh


#include "RocketquakewGameStateBase.h"

#include "Net/UnrealNetwork.h"

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

void ARocketquakewGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARocketquakewGameStateBase, RoundCountDown);
    DOREPLIFETIME(ARocketquakewGameStateBase, CurrentRound);
    DOREPLIFETIME(ARocketquakewGameStateBase, Rounds);
}
