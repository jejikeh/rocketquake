// Rocketquake, jejikeh

#pragma once

UENUM(BlueprintType)
enum class ERocketquakeMatchState : uint8
{
    WaitingStart,
    InProgress,
    GameOver
};

UENUM(BlueprintType)
enum class EPlayerGameState
{
    Playing,
    Paused,
    GameOver
};