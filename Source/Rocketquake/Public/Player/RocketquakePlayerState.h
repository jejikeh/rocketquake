// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Types/GameMatchStates.h"
#include "RocketquakePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerGameStateChanged, EPlayerGameState, NewState);

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakePlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    void SetTeamID(int32 ID)
    {
        TeamID = ID;
    }
    
    int32 GetTeamID() const
    {
        return TeamID;
    }

    void SetTeamColor(const FLinearColor& NewTeamColor)
    {
        TeamColor = NewTeamColor;
    }
    
    FLinearColor GetTeamColor() const
    {
        return TeamColor;
    }
    
    void AddKill()
    {
        KillsNum++;
    }

    void AddDeath()
    {
        DeathNus++;
    }

    int32 GetKillsNum() const
    {
        return KillsNum;
    }

    int32 GetDeathNum() const
    {
        return DeathNus;
    }

    void SetCurrentGameState(EPlayerGameState NewState);

    EPlayerGameState GetCurrentGameState() const
    {
        return CurrentGameState;
    }

    void LogInfo();

    FOnPlayerGameStateChanged OnPlayerGameStateChanged;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    
    virtual void BeginPlay() override;

private:
    UPROPERTY(Replicated)
    int32 TeamID;

    UPROPERTY(Replicated)
    FLinearColor TeamColor;

    UPROPERTY(Replicated)
    int32 KillsNum = 0;

    UPROPERTY(Replicated)
    int32 DeathNus = 0;

    EPlayerGameState CurrentGameState;

    UFUNCTION()
    void OnMatchStateChanged(ERocketquakeMatchState NewState);
};
