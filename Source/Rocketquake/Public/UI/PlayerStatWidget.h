// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API UPlayerStatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPlayerName(const FString& Name);

    void SetKills(const int32 Num);

    void SetDeaths(const int32 Num);

    void SetScore(const int32 Num);

    void SetPlayerIndicatorVisible(bool bVisible);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerNameTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillsTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DeathsTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* PlayerImageImage;
};
