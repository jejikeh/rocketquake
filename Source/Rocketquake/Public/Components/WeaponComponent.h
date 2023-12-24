// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pickup/AmmoPickup.h"
#include "Weapon/RocketquakeWeapon.h"
#include "WeaponComponent.generated.h"


struct FWeaponUIData;
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

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ResetAnimNotifies();
    void Multicast_ResetAnimNotifies_Implementation();
    
    UFUNCTION(Server, Reliable)
    void Reload();
    void Reload_Implementation();

    bool GetWeaponUIData(FWeaponUIData& WeaponUIData) const;

    bool GetWeaponAmmoData(FAmmoData &AmmoData);

    bool TryToAddAmmo(TSubclassOf<ARocketquakeWeapon> Class, int32 AmmoCount);

    void Zoom(bool Enabled);

    void ResetZoom();

    ARocketquakeWeapon* GetCurrentWeapon() const
    {
        return CurrentWeapon;
    }

protected:
    virtual void BeginPlay() override;
    
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TArray<FWeaponData> WeaponClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* EquipAnimMontage;

    // @NOTE: This is for UI on client side
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FWeaponUIData DefaultWeaponUIData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmoData;

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

private:
    UFUNCTION(Server, Reliable)
    void SpawnWeapons();
    void SpawnWeapons_Implementation();

    UFUNCTION(Server, Reliable)
    void EquipWeapon(int32 Index);
    void EquipWeapon_Implementation(int32 Index);

    UPROPERTY(Replicated)
    ARocketquakeWeapon* CurrentWeapon;

    UPROPERTY()
    TArray<ARocketquakeWeapon*> Weapons;

    UPROPERTY()
    UAnimMontage* CurrentAnimMontage = nullptr;

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayAnimMontage(UAnimMontage* AnimMontage);
    void Multicast_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage);

    UFUNCTION(Server, Unreliable)
    void Server_PlayReloadAnimMontage();
    void Server_PlayReloadAnimMontage_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayReloadAnimMontage();
    void Multicast_PlayReloadAnimMontage_Implementation();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastSkipOwner_PlayEquipAnimMontage();
    void MulticastSkipOwner_PlayEquipAnimMontage_Implementation();

    // NOTE(jejikeh): Replace this with replicated property
    UFUNCTION(Server, Reliable)
    void Server_SetReloadAnimationInProgress(bool InProgress);
    void Server_SetReloadAnimationInProgress_Implementation(bool InProgress);

    // NOTE(jejikeh): Replace this with replicated property
    UFUNCTION(Server, Reliable)
    void Server_SetEquippedAnimationInProgress(bool InProgress);
    void Server_SetEquippedAnimationInProgress_Implementation(bool InProgress);

    UFUNCTION(Client, Reliable)
    void Client_DebugPrintAnimationsInformation();
    void Client_DebugPrintAnimationsInformation_Implementation();

    UFUNCTION(Client, Reliable)
    void Client_SetCurrentWeaponUIData(FWeaponUIData WeaponUIData);
    void Client_SetCurrentWeaponUIData_Implementation(FWeaponUIData WeaponUIData);

    UFUNCTION(Server, Reliable)
    void Server_SetClientWeaponAmmoData();
    void Server_SetClientWeaponAmmoData_Implementation();
    
    UFUNCTION(Client, Reliable)
    void Client_SetCurrentWeaponAmmoData(FAmmoData AmmoData);
    void Client_SetCurrentWeaponAmmoData_Implementation(FAmmoData AmmoData);

    void InitAnimations();
    
    void OnEquipFinished(USkeletalMeshComponent* MeshComp);

    void OnReloadFinished(USkeletalMeshComponent* MeshComp);

    UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponIndex)
    int32 CurrentWeaponIndex = 0;

    UFUNCTION()
    void OnRep_CurrentWeaponIndex();
    
    FWeaponUIData CurrentWeaponUIData;

    FAmmoData CurrentAmmoData;
    
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

    bool bIsFirstTick = true;
};
