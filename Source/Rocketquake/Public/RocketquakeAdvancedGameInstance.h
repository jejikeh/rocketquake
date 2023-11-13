// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "RocketquakeAdvancedGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API URocketquakeAdvancedGameInstance : public UAdvancedFriendsGameInstance
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

    UFUNCTION()
    void OnCreateSessionComplete();
    
    UFUNCTION(BlueprintCallable)
    void CreateSession();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MainMapName = NAME_None;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MenuMapName = NAME_None;

    virtual void Init() override;
};
