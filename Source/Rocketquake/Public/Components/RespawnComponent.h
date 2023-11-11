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

    void Respawn(int RespawnTime);

private:
    FTimerHandle RespawnTimerHandle;

    int32 RespawnCountDown;

    void RespawnTimerUpdate();
};
