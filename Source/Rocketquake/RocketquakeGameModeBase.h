// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/GameMatchStates.h"
#include "RocketquakeGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMathChanged);

USTRUCT(BlueprintType)
struct FGameData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    int32 Players;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    int32 Rounds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    int32 RoundTimeSecs;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    FLinearColor DefaultTeamColor = FLinearColor::Yellow;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    TArray<FLinearColor> TeamColors;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    int32 RespawnTime = 5;
};

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakeGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ARocketquakeGameModeBase();

    virtual void StartPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FGameData GameData;

    void Killed(AController* Killer, AController* Victim);

    UFUNCTION(BlueprintCallable)
    void RestartGame();

    UPROPERTY(BlueprintAssignable)
    FOnMathChanged OnMatchChanged;

    FGameData GetGameData() const
    {
        return GameData;
    }

    int32 GetCurrentRound() const
    {
        return CurrentRound;
    }

    int32 GetRoundCountDown() const
    {
        return RoundCountDown;
    }

    void RespawnRequest(AController* Controller);

protected:
    virtual void GenericPlayerInitialization(AController *C) override;

private:
    // Spawn players?

    ERocketquakeMatchState CurrentMatchState = ERocketquakeMatchState::WaitingStart;
    
    int32 CurrentRound = 1;

    int32 RoundCountDown = 0;

    FTimerHandle GameRoundTimerHandle;

    void StartRound();

    void GameTimerUpdate();

    void ResetPlayers();
    
    void ResetOnePlayer(AController* Controller);

    void CreateTeamsInfo();

    FLinearColor GetColorByTeamId(int32 TeamId) const;

    void SetPlayerColor(AController *Controller);

    void StartRespawn(const AController* Controller) const;

    void GameOver();

    void SetMatchState(ERocketquakeMatchState NewState);
};
