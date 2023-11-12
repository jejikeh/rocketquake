// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RocketquakeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API URocketquakeGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    FName GetMainMapName() const
    {
        return MainMapName;
    }

    FName GetMenuMapName() const
    {
        return MenuMapName;
    }
    
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MainMapName = NAME_None;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MenuMapName = NAME_None;
};
