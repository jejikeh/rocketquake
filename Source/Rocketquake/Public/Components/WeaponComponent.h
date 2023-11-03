// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


class ARocketquakeWeapon;
class UEquipFinishedAnimNotify;

USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ARocketquakeWeapon> WeaponClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMontage;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKETQUAKE_API UWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponComponent();

    UFUNCTION(Server, Reliable)
    void StartShoot();
    void StartShoot_Implementation();

    UFUNCTION(Server, Reliable)
    void StopShoot();
    void StopShoot_Implementation();

    UFUNCTION(Server, Reliable)
    void NextWeapon();
    void NextWeapon_Implementation();

    UFUNCTION(Server, Reliable)
    void Reload();
    void Reload_Implementation();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TArray<FWeaponData> WeaponClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* EquipAnimMontage;

private:
    UFUNCTION(Server, Reliable)
    void SpawnWeapons();
    void SpawnWeapons_Implementation();

    UFUNCTION(Server, Reliable)
    void EquipWeapon(int32 Index);
    void EquipWeapon_Implementation(int32 Index);
    
    UPROPERTY()
    ARocketquakeWeapon* CurrentWeapon;

    UPROPERTY()
    TArray<ARocketquakeWeapon*> Weapons;

    UPROPERTY()
    UAnimMontage* CurrentAnimMontage = nullptr;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayAnimMontage(UAnimMontage* AnimMontage);
    void Multicast_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage);

    void InitAnimations();
    
    void OnEquipFinished(USkeletalMeshComponent* MeshComp);

    void OnReloadFinished(USkeletalMeshComponent* MeshComp);

    int32 CurrentWeaponIndex = 0;

    bool bIsEquippedInProgress = false;
    
    bool bIsReloadInProgress = false;

    bool CanEquip() const;

    bool CanShoot() const;
    
    bool CanReload() const;

    void OnEmptyClip();

    void ChangeClip();

    template <typename T>
    T* FindNotifyByClass(UAnimSequenceBase* AnimSequence)
    {
        if (AnimSequence)
        {
            for (auto Notify : AnimSequence->Notifies)
            {
                auto AnimNotify = Cast<T>(Notify.Notify);
                if (AnimNotify)
                {
                    return AnimNotify;
                }
            }
        }

        return nullptr;
    }
};
