// Rocketquake, jejikeh


#include "Menu/MenuGameModeBase.h"

#include "Menu/MenuHUD.h"
#include "Menu/MenuPlayerController.h"

AMenuGameModeBase::AMenuGameModeBase()
{
    PlayerControllerClass = AMenuPlayerController::StaticClass();
    HUDClass = AMenuHUD::StaticClass();
}
