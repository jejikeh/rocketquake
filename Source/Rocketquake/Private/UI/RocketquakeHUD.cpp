// Rocketquake, jejikeh


#include "UI/RocketquakeHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"

void ARocketquakeHUD::DrawHUD()
{
    Super::DrawHUD();
}

void ARocketquakeHUD::BeginPlay()
{
    Super::BeginPlay();

    if (const auto PlayerHudWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHudWidgetClass))
    {
        PlayerHudWidget->AddToViewport();
    }
}
