// Rocketquake, jejikeh


#include "Actor/DamageSphereActor.h"

// Sets default values
ADamageSphereActor::ADamageSphereActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADamageSphereActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageSphereActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

