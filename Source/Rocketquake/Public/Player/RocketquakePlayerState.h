// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Types/GameMatchStates.h"
#include "RocketquakePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerGameStateChanged, EPlayerGameState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillPlayer, FString, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillPlayerSim, FString, Killer, FString, Victim);


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
    
    void AddKill(const FString& Victim);

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

    FOnPlayerGameStateChanged OnPlayerGameStateChanged;

    UPROPERTY(BlueprintAssignable)
    FOnKillPlayer OnKillPlayer;

    UPROPERTY(BlueprintAssignable)
    FOnKillPlayerSim OnKillPlayerSim;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_BroadcastOnKillPlayer(const FString& Victim);
    void Multicast_BroadcastOnKillPlayer_Implementation(const FString& Victim);

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
