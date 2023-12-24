// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Types/GameMatchStates.h"
#include "GameFramework/GameStateBase.h"
#include "RocketquakewGameStateBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, ERocketquakeMatchState, NewState);

/**
 * This is mistake, but im too lazy to change name :)
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakewGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
    FOnMatchStateChanged OnMatchStateChanged;
    
    void UpdateRoundCountDown(int32 NewRoundCountDown);

    void UpdateCurrentRound(int32 NewCurrentRound);

    void SetRounds(int32 NewRounds);

    int32 GetCurrentRound() const
    {
        return CurrentRound;
    }

    int32 GetRoundCountDown() const
    {
        return RoundCountDown;
    }

    int32 GetRounds() const
    {
        return Rounds;
    }

    ERocketquakeMatchState GetCurrentMatchState() const
    {
        return CurrentMatchState;
    }

    UFUNCTION()
    void SetCurrentMatchState(ERocketquakeMatchState NewState);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_EndGame();
    void Multicast_EndGame_Implementation();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    int32 CurrentRound = 0;

    UPROPERTY(Replicated)
    int32 RoundCountDown = 0;

    UPROPERTY(Replicated)
    int32 Rounds = 0;

    UPROPERTY(ReplicatedUsing=OnRep_CurrentMatchState)
    ERocketquakeMatchState CurrentMatchState;

    UFUNCTION()
    virtual void OnRep_CurrentMatchState();
};
