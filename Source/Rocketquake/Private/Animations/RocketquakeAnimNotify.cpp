// Rocketquake, jejikeh


#include "Animations/RocketquakeAnimNotify.h"

void URocketquakeAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    OnNotified.Broadcast(MeshComp);
    
    Super::Notify(MeshComp, Animation);
}
