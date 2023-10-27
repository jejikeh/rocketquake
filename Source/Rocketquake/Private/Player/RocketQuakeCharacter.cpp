// Rocketquake, jejikeh


#include "Player/RocketQuakeCharacter.h"


// Sets default values
ARocketQuakeCharacter::ARocketQuakeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARocketQuakeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARocketQuakeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARocketQuakeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

