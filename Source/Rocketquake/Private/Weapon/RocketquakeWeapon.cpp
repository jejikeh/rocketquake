// Rocketquake, jejikeh


#include "Weapon/RocketquakeWeapon.h"

// Sets default values
ARocketquakeWeapon::ARocketquakeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);

}

void ARocketquakeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

