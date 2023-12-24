// Rocketquake, jejikeh


#include "UI/PlayerStatWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerStatWidget::SetPlayerName(const FString &Name)
{
    if (PlayerNameTextBlock)
    {
        PlayerNameTextBlock->SetText(FText::FromString(Name));
    }
}

void UPlayerStatWidget::SetKills(const int32 Num)
{
    if (KillsTextBlock)
    {
        KillsTextBlock->SetText(FText::AsNumber(Num));
    }
}

void UPlayerStatWidget::SetDeaths(const int32 Num)
{
    if (DeathsTextBlock)
    {
        DeathsTextBlock->SetText(FText::AsNumber(Num));
    }
}

void UPlayerStatWidget::SetScore(const int32 Num)
{
    if (ScoreTextBlock)
    {
        ScoreTextBlock->SetText(FText::AsNumber(Num));
    }
}

void UPlayerStatWidget::SetPlayerIndicatorVisible(bool bVisible)
{
    if (PlayerImageImage)
    {
        PlayerImageImage->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}
