// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/GameMatchStates.h"
#include "RocketquakePlayerController.generated.h"

class UInputAction;
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

    virtual void SetupInputComponent() override;

    virtual void BeginPlay() override;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    UInputAction* PauseAction;

private:
    void OnPauseGame();

    UFUNCTION()
    void OnPlayerStateChanged(EPlayerGameState PlayerGameState);

    FTimerHandle WaitPlayerStateTimer;

    void WaitPlayerState();

    UFUNCTION(Client, Reliable)
    void Client_SetInputMode();
    void Client_SetInputMode_Implementation();
};
