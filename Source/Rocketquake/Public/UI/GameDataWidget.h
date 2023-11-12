// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameDataWidget.generated.h"

class ARocketquakewGameStateBase;
class ARocketquakePlayerState;
class ARocketquakeGameModeBase;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UGameDataWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    int32 GetKillsNum() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    int32 GetCurrentRoundNum() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    int32 GetTotalRoundsNum() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    int32 GetRoundCountDown() const;
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    int32 GetDeathsCount() const;

private:
    ARocketquakeGameModeBase* GetGameMode() const;

    ARocketquakePlayerState* GetPlayerState() const;

    ARocketquakewGameStateBase* GetGameState() const;
};
