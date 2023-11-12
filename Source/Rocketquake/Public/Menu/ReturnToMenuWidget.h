// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UReturnToMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* ReturnToMenuButton;

    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnReturnToMenuButtonClicked();
};
