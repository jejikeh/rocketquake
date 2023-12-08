// Rocketquake, jejikeh


#include "RocketquakeGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

void URocketquakeGameInstance::Init()
{
    Super::Init();

    // if (const auto OnlineSubSystem = IOnlineSubsystem::Get())
    // {
    //     SessionInterface = OnlineSubSystem->GetSessionInterface();
    //     if (SessionInterface.IsValid())
    //     {
    //         SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &URocketquakeGameInstance::OnCreateSessionComplete);
    //         SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &URocketquakeGameInstance::OnFindSessionsComplete);
    //         SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &URocketquakeGameInstance::OnJoinSessionComplete);
    //     }
    // }
}

void URocketquakeGameInstance::CreateSession()
{
    FOnlineSessionSettings SessionSettings;

    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
    
    SessionSettings.bUsesPresence = true;
    SessionSettings.NumPublicConnections = 4;

    SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

void URocketquakeGameInstance::JoinMatchSession()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());

    SessionSearch->MaxSearchResults = 10000;
    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");

    if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
    {
        // SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
    }
    
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void URocketquakeGameInstance::OnCreateSessionComplete(FName Name, bool bSuccess)
{
    if (bSuccess)
    {
        GetWorld()->ServerTravel("/Game/Resources/Levels/L_Showcase?listen");
    }
}

void URocketquakeGameInstance::OnFindSessionsComplete(bool bSuccess)
{
    UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete"));
    if (bSuccess && SessionSearch.IsValid())
    {
        TArray<FOnlineSessionSearchResult> SessionSearchResults = SessionSearch->SearchResults;
        if (SessionSearchResults.Num())
        {
            SessionInterface->JoinSession(0, "My Session", SessionSearchResults[0]);
        }
    }
}

void URocketquakeGameInstance::OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
    if (const auto PlayerController =  UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FString ServerURL = "";
        SessionInterface->GetResolvedConnectString("My Session", ServerURL);
        if (ServerURL != "")
        {
            PlayerController->ClientTravel(ServerURL, ETravelType::TRAVEL_Absolute);
        }
    }
}
