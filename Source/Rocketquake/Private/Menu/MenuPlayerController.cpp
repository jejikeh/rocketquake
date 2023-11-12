// Rocketquake, jejikeh


#include "Menu/MenuPlayerController.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
