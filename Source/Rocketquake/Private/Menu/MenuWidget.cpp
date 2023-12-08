// Rocketquake, jejikeh


#include "Menu/MenuWidget.h"

#include "RocketquakeAdvancedGameInstance.h"
#include "RocketquakeGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // if (StartButton)
    // {
    //     StartButton->OnClicked.AddDynamic(this, &UMenuWidget::OnStartButtonClicked);
    // }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UMenuWidget::OnExitButtonClicked);
    }

    if (HostServerButton)
    {
        HostServerButton->OnClicked.AddDynamic(this, &UMenuWidget::OnHostServerClicked);
    }
    
    if (ExploreServersButton)
    {
        ExploreServersButton->OnClicked.AddDynamic(this, &UMenuWidget::OnExploreServersClicked);
    }
}

void UMenuWidget::OnStartButtonClicked()
{
    const auto GameInstance = GetWorld()->GetGameInstance<URocketquakeAdvancedGameInstance>();
    if (!GameInstance)
    {
        return;
    }

    const auto MainMapName = GameInstance->GetMainMapName();

    if (MainMapName.IsNone())
    {
        return;
    }

    UGameplayStatics::OpenLevel(this, MainMapName);
}

void UMenuWidget::OnExitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UMenuWidget::OnHostServerClicked()
{
    const auto GameInstance = GetWorld()->GetGameInstance<URocketquakeAdvancedGameInstance>();
    if (!GameInstance)
    {
        return;
    }

    // GameInstance->CreateSession();
}

void UMenuWidget::OnExploreServersClicked()
{
    const auto GameInstance = GetWorld()->GetGameInstance<URocketquakeAdvancedGameInstance>();
    if (!GameInstance)
    {
        return;
    }

    // GameInstance->JoinMatchSession();
}
