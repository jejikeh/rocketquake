// Rocketquake, jejikeh


#include "Weapon/LauncherProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/WeaponFXComponent.h"
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

    // DrawDebugSphere(GetWorld(), GetActorLocation(), RadialForceComponent->Radius, 12, FColor::Red, false, 5.0f);
    WeaponFXComponent->PlayImpactFx(Hit);

    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        BaseDamage,
        Hit.Location,
        RadialForceComponent->Radius,
        {},
        {},
         this);

    const auto Player = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (!Player)
    {
        return;
    }

    Player->PlayerCameraManager->StartCameraShake(CameraShake);
    
    Destroy();
}

void ALauncherProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    SphereComponent->OnComponentHit.AddDynamic(this, &ALauncherProjectile::OnHit);
    SphereComponent->IgnoreActorWhenMoving(GetOwner(), true);

    SetLifeSpan(10.0f);
}

void ALauncherProjectile::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
    FVector NormalImpulse, const FHitResult &Hit)
{
    if (HasAuthority())
    {
        HandleHit(Hit);
    }
    
    // ProjectileMovementComponent->StopMovementImmediately();
    // RadialForceComponent->FireImpulse();
    //
    // DrawDebugSphere(GetWorld(), GetActorLocation(), RadialForceComponent->Radius, 12, FColor::Red, false, 5.0f);
    //
    // Destroy();
}
