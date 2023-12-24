// Rocketquake, jejikeh


#include "RocketquakeAdvancedGameInstance.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/GameplayStatics.h"

void URocketquakeAdvancedGameInstance::Init()
{
    Super::Init();

    // auto AdvancedSessions = UCreateSessionCallbackProxyAdvanced();
    // AdvancedSessions.CreateAdvancedSession()
}

void URocketquakeAdvancedGameInstance::CreateSession()
{
    // const auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    // if (!PlayerController)
    // {
    //     return;
    // }
    //
    // const auto SessionCallback = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(GetWorld(),{}, PlayerController);
    // SessionCallback->OnSuccess.AddDynamic(this, &URocketquakeAdvancedGameInstance::OnCreateSessionComplete);
}

void URocketquakeAdvancedGameInstance::SetMouseZoomSensitivity(float ZoomSensitivity)
{
    SlowdownZoomFactor = ZoomSensitivity;
}

void URocketquakeAdvancedGameInstance::SetMouseSensitivity(float Sensitivity)
{
    MouseSensitivity = Sensitivity;
}

void URocketquakeAdvancedGameInstance::OnCreateSessionComplete()
{
    GetWorld()->ServerTravel("/Game/Resources/Levels/L_Showcase?listen");
}
