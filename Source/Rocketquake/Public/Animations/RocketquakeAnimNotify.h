// Rocketquake, jejikeh

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RocketquakeAnimNotify.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifiedSignature, USkeletalMeshComponent*)

/**
 * 
 */
UCLASS()
class ROCKETQUAKE_API URocketquakeAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

    FOnNotifiedSignature OnNotified;
};
