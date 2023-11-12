// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Types/GameMatchStates.h"
#include "RocketquakeHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakeHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHudWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnPlayerGameStateChanged(EPlayerGameState NewState);

    UPROPERTY()
    TMap<EPlayerGameState, UUserWidget*> GameWidgets;

    UPROPERTY()
    UUserWidget* CurrentWidget = nullptr;

    FTimerHandle WaitPlayerStateTimer;

    void WaitPlayerState();
};
