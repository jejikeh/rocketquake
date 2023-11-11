// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RocketquakewGameStateBase.generated.h"

/**
 * This is mistake, but im too lazy to change name :)
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakewGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
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

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
private:
    UPROPERTY(Replicated)
    int32 CurrentRound = 0;

    UPROPERTY(Replicated)
    int32 RoundCountDown = 0;

    UPROPERTY(Replicated)
    int32 Rounds = 0;
};
