// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKETQUAKE_API URespawnComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URespawnComponent();

    UFUNCTION(Client, Unreliable)
    void Client_Respawn(int RespawnTime);
    void Client_Respawn_Implementation(int RespawnTime);

    UFUNCTION(Client, Unreliable)
    void Client_ResetRespawnTimer();
    void Client_ResetRespawnTimer_Implementation();

    int32 GetRespawnCountDown() const
    {
        return RespawnCountDown;
    }

    bool IsRespawnInProgress() const;

protected:
    
private:
    FTimerHandle RespawnTimerHandle;

    int32 RespawnCountDown;

    void RespawnTimerUpdate();
};
