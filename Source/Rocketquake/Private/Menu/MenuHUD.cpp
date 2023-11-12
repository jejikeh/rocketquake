// Rocketquake, jejikeh


#include "Menu/MenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "Menu/MenuWidget.h"

class UMenuWidget;

void AMenuHUD::BeginPlay()
{
    Super::BeginPlay();

    if (MenuWidgetClass)
    {
        if (const auto MenuWidget = CreateWidget<UMenuWidget>(GetWorld(), MenuWidgetClass))
        {
            MenuWidget->AddToViewport();
        }
    }
}
