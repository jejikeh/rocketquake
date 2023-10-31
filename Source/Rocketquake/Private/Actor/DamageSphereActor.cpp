// Rocketquake, jejikeh


#include "Actor/DamageSphereActor.h"

#include "Kismet/GameplayStatics.h"

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

	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, Color, true);
}

// Called every frame
void ADamageSphereActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), Radius, DamageType, {}, this, nullptr, bDoFullDamage);
}

