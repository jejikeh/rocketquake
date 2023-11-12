// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/GameMatchStates.h"
#include "GameOverWidget.generated.h"

class UButton;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* PlayerStatBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PlayerStatWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UButton* RestartButton;

private:
    UFUNCTION()
    void OnPlayerGameStateChanged(EPlayerGameState NewState);
    
    void UpdatePlayerStats();

    FTimerHandle WaitPlayerStateTimer;

    void WaitPlayerState();

    UFUNCTION()
    void OnRestartButtonClicked();
};
 