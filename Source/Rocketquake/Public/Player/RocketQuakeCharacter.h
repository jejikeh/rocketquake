// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RocketQuakeCharacter.generated.h"

UCLASS()
class ROCKETQUAKE_API ARocketQuakeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARocketQuakeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
