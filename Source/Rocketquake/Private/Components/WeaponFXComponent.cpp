// Rocketquake, jejikeh


#include "Components/WeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UWeaponFXComponent::UWeaponFXComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponFXComponent::PlayImpactFx(const FHitResult &Hit)
{
    auto ImpactData = DefaultImpactData;

    if (Hit.PhysMaterial.IsValid())
    {
        const auto PhysMat = Hit.PhysMaterial.Get();

        if (EffectsMap.Contains(PhysMat))
        {
            ImpactData = EffectsMap[PhysMat];
        }
    }

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        ImpactData.NiagaraSystem,
        Hit.ImpactPoint,
        Hit.ImpactNormal.Rotation());

    auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
        GetWorld(),
        ImpactData.DecalData.DecalMaterial,
        ImpactData.DecalData.Size,
        Hit.ImpactPoint,
        Hit.ImpactNormal.Rotation());

    if (DecalComponent)
    {
        DecalComponent->SetFadeOut(ImpactData.DecalData.FadeTime, ImpactData.DecalData.LifeTime);
    }
}

void UWeaponFXComponent::BeginPlay()
{
    Super::BeginPlay();
}
