// Rocketquake, jejikeh


#include "Menu/MenuPlayerController.h"

void AMenuPlayerController::SetMenuWidget(UUserWidget *Widget)
{
    MenuWidget = Widget;
}

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
