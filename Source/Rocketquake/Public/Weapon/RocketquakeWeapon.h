// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketquakeWeapon.generated.h"

class USoundCue;
class UNiagaraComponent;
class UNiagaraSystem;
DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature);

USTRUCT(BlueprintType)
struct FAmmoData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!InfiniteAmmo"))
    int32 Clips;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool InfiniteAmmo;
};

USTRUCT(BlueprintType)
struct FWeaponUIData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UTexture2D* Crosshair;
};

UCLASS()
class ROCKETQUAKE_API ARocketquakeWeapon : public AActor
{
    GENERATED_BODY()

public:
    ARocketquakeWeapon();

    virtual void StartShoot();

    virtual void StopShoot();

    virtual void ForceStopShot();

    virtual void Zoom(bool Enabled) {}

    virtual void ResetZoom() {}

    UFUNCTION(Server, Reliable)
    void ChangeClip();
    void ChangeClip_Implementation();

    bool CanReload() const;
    
    bool AddAmmo(int32 AmmoCount);

    FOnClipEmptySignature OnClipEmpty;

    UFUNCTION(BlueprintCallable, Category = "UI")
    FWeaponUIData GetWeaponUIData() const
    {
        return WeaponUIData;
    };

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    FAmmoData GetAmmoData() const
    {
        return CurrentAmmoData;
    };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* NiagaraSystem;

    UNiagaraComponent* SpawnMuzzleFlash() const;

protected:
    virtual void BeginPlay() override;

    void MakeHit(FHitResult& HitResult, const FVector& Vector, const FVector& TraceEnd) const;

    virtual void MakeShot();

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    AController* GetController() const;

    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float Damage = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData AmmoDefaultData{15, 10, false};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FWeaponUIData WeaponUIData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
    USoundCue* FireSound;

    UFUNCTION(Server, Reliable)
    void Server_DecreaseAmmo();
    void Server_DecreaseAmmo_Implementation();
    
    bool IsAmmoEmpty();

    bool IsClipEmpty() const;

private:
    FAmmoData CurrentAmmoData;

    UPROPERTY(Replicated)
    bool bServerToldMeWhatImOutOfBullets = false;
};
