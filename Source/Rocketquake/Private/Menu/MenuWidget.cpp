// Rocketquake, jejikeh


#include "Menu/MenuWidget.h"

#include "RocketquakeGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UMenuWidget::OnStartButtonClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UMenuWidget::OnExitButtonClicked);
    }
}

void UMenuWidget::OnStartButtonClicked()
{
    const auto GameInstance = GetWorld()->GetGameInstance<URocketquakeGameInstance>();
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
