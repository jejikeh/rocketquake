// Rocketquake, jejikeh


#include "Player/RocketquakePlayerState.h"

#include "Net/UnrealNetwork.h"

void ARocketquakePlayerState::LogInfo()
{
    UE_LOG(LogTemp, Error, TEXT("TeamID: %d, Kills: %d, Deaths: %d"), TeamID, KillsNum, DeathNus);
}

void ARocketquakePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARocketquakePlayerState, TeamID);
    DOREPLIFETIME(ARocketquakePlayerState, KillsNum);
    DOREPLIFETIME(ARocketquakePlayerState, DeathNus);
    DOREPLIFETIME(ARocketquakePlayerState, TeamColor);
}
