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

    UFUNCTION(BlueprintCallable)
    void SetMouseZoomSensitivity(float ZoomSensitivity);

    UFUNCTION(BlueprintCallable)
    void SetMouseSensitivity(float Sensitivity);

    UFUNCTION(BlueprintCallable)
    float GetMouseSensitivity() const
    {
        return MouseSensitivity;
    }

    UFUNCTION(BlueprintCallable)
    float GetSlowdownZoomFactor() const
    {
        return SlowdownZoomFactor;
    }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MainMapName = NAME_None;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MenuMapName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    float SlowdownZoomFactor = 2.0f;

    virtual void Init() override;
};
