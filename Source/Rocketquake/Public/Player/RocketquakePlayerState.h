// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RocketquakePlayerState.generated.h"

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

    void LogInfo();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    int32 TeamID;

    UPROPERTY(Replicated)
    FLinearColor TeamColor;

    UPROPERTY(Replicated)
    int32 KillsNum = 0;

    UPROPERTY(Replicated)
    int32 DeathNus = 0;
};
