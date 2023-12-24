// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API AMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    UUserWidget* GetMenuWidget() const
    {
        return MenuWidget;
    }

    UFUNCTION()
    void SetMenuWidget(UUserWidget* Widget);
    
protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UUserWidget* MenuWidget;
};
