// Rocketquake, jejikeh


#include "UI/RocketquakeHUD.h"

#include "Engine/Canvas.h"

void ARocketquakeHUD::DrawHUD()
{
    Super::DrawHUD();

    DrawCrosshair();
}

void ARocketquakeHUD::DrawCrosshair()
{
    const TInterval<float>  Center(Canvas->SizeX / 2, Canvas->SizeY / 2);
    constexpr float LineThickness = 2.0f;
    constexpr float LineHalfSize = 10.0f;
    const FLinearColor LineColor = FLinearColor::Green;
    
    DrawLine(Center.Min - LineHalfSize, Center.Max, Center.Min + LineHalfSize, Center.Max, LineColor, LineThickness);
    DrawLine(Center.Min, Center.Max - LineHalfSize, Center.Min, Center.Max + LineHalfSize, LineColor, LineThickness);
}
