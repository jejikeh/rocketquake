// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RocketquakePlayerController.generated.h"

class URespawnComponent;
/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API ARocketquakePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARocketquakePlayerController();
    
    void StartSpectating();

    void StartPlaying();

protected:
    UFUNCTION(Client, Reliable)
    void Client_StartSpectating();
    void Client_StartSpectating_Implementation();

    virtual void OnRep_Pawn() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
    URespawnComponent* RespawnComponent;
};
