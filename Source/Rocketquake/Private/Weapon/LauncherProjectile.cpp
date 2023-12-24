// Rocketquake, jejikeh


#include "Weapon/LauncherProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/WeaponFXComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ALauncherProjectile::ALauncherProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComponent->InitSphereRadius(5.0f);
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
    SphereComponent->bReturnMaterialOnMove = true;
    SetRootComponent(SphereComponent);

    RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("RadialForceComponent");
    RadialForceComponent->SetupAttachment(SphereComponent);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    ProjectileMovementComponent->SetIsReplicated(true);

    WeaponFXComponent = CreateDefaultSubobject<UWeaponFXComponent>("WeaponFXComponent");
}

void ALauncherProjectile::SetShootDirection_Implementation(const FVector &Direction)
{
    ShootDirection = Direction;
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void ALauncherProjectile::HandleHit_Implementation(FHitResult Hit)
{
    ProjectileMovementComponent->StopMovementImmediately();
    RadialForceComponent->FireImpulse();
    WeaponFXComponent->PlayImpactFx(Hit);
    
    const auto Player = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (!Player)
    {
        return;
    }

    Player->PlayerCameraManager->StartCameraShake(CameraShake);
    
    Destroy();
}

void ALauncherProjectile::Server_ApplyDamageToActors_Implementation(FVector Locaction)
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        return;
    }
    
    const auto Controller = Cast<APlayerController>(Player->GetController());
    if (!Controller)
    {
        return;
    }

    TArray<AActor*> OutActors;
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        Locaction,
        RadialForceComponent->Radius,
        TraceObjectTypes, {}, {}, OutActors);

    for (const auto Actor : OutActors)
    {
        if (!Actor)
        {
            continue;
        }
        
        Actor->TakeDamage(FMath::RandRange(BaseDamage / 4, BaseDamage), FDamageEvent(), Controller, this);
    }
}

void ALauncherProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    SphereComponent->OnComponentHit.AddDynamic(this, &ALauncherProjectile::OnHit);
    // SphereComponent->IgnoreActorWhenMoving(GetOwner(), true);

    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
    
    SetLifeSpan(10.0f);
}

void ALauncherProjectile::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
    FVector NormalImpulse, const FHitResult &Hit)
{
    Server_ApplyDamageToActors(Hit.Location);
    
    if (HasAuthority())
    {
        HandleHit(Hit);
    }
}
