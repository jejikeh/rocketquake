// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "RocketquakeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API URocketquakeGameInstance : public UAdvancedFriendsGameInstance
{
    GENERATED_BODY()

public:
    FName GetMainMapName() const
    {
        return MainMapName;
    }

    FName GetMenuMapName() const
    {
        return MenuMapName;
    }

    UFUNCTION(BlueprintCallable)
    void CreateSession();

    UFUNCTION(BlueprintCallable)
    void JoinMatchSession();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MainMapName = NAME_None;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FName MenuMapName = NAME_None;

    UFUNCTION()
    void OnCreateSessionComplete(FName Name, bool bSuccess);

    UFUNCTION()
    void OnFindSessionsComplete(bool bSuccess);

    void OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result);
    
    virtual void Init() override;

    IOnlineSessionPtr SessionInterface;

    TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
