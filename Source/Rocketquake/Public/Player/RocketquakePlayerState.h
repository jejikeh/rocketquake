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

private:
    int32 TeamID;
    FLinearColor TeamColor;
};
