// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;
    
    UPROPERTY(meta = (BindWidget))
    UButton* ExitButton;

    UPROPERTY(meta = (BindWidget))
    UButton* HostServerButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ExploreServersButton;

private:
    UFUNCTION()
    void OnStartButtonClicked();

    UFUNCTION()
    void OnExitButtonClicked();

    UFUNCTION()
    void OnHostServerClicked();
    
    UFUNCTION()
    void OnExploreServersClicked();
};
