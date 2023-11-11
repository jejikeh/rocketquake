// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpectatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API USpectatorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool GetRespawnTime(int& RespawnTime) const;
};
