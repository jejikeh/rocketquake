// Rocketquake, jejikeh


#include "Menu/ReturnToMenuWidget.h"

#include "RocketquakeAdvancedGameInstance.h"
#include "RocketquakeGameInstance.h"
#include "Components/Button.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"

class URocketquakeGameInstance;

void UReturnToMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ReturnToMenuButton)
    {
        ReturnToMenuButton->OnClicked.AddDynamic(this, &UReturnToMenuWidget::OnReturnToMenuButtonClicked);
    }
}

void UReturnToMenuWidget::OnReturnToMenuButtonClicked()
{
    const auto GameInstance = GetWorld()->GetGameInstance<URocketquakeAdvancedGameInstance>();
    if (!GameInstance)
    {
        return;
    }

    const auto MenuMapName = GameInstance->GetMenuMapName();

    if (MenuMapName.IsNone())
    {
        return;
    }

    UGameplayStatics::OpenLevel(this, MenuMapName);
}
