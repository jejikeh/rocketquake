// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageSphereActor.generated.h"

class UProjectileMovementComponent;
class UMovementComponent;
class UDamageType;

UCLASS()
class ROCKETQUAKE_API ADamageSphereActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageSphereActor();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor Color = FColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDoFullDamage = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UProjectileMovementComponent* ProjectileMovementComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
