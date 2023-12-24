// Rocketquake, jejikeh


#include "Menu/MenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "Menu/MenuPlayerController.h"
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
            if (const auto Controller = Cast<AMenuPlayerController>(GetOwningPlayerController()))
            {
                Controller->SetMenuWidget(MenuWidget);
            }
        }
    }
}
