// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFXComponent.generated.h"

class USoundCue;
class UNiagaraSystem;
class UPhysicalMaterial;

USTRUCT(BlueprintType)
struct FDecalData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    UMaterialInterface* DecalMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    FVector Size = FVector(10.0f, 10.0f, 10.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    float LifeTime = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    float FadeTime = 0.7f;
};

USTRUCT(BlueprintType)
struct FImpactData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    UNiagaraSystem* NiagaraSystem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    FDecalData DecalData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    USoundCue* ImpactSound;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKETQUAKE_API UWeaponFXComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponFXComponent();

    void PlayImpactFx(const FHitResult &Hit);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    FImpactData DefaultImpactData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponFX")
    TMap<UPhysicalMaterial*, FImpactData> EffectsMap;
};
