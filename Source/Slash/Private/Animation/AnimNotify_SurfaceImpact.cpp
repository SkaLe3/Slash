// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_SurfaceImpact.h"
#include "Characters/BaseCharacter.h"

void UAnimNotify_SurfaceImpact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(Character);
	if (BaseCharacter)
	{
		FSurfaceImpactEventData ImpactData = FSurfaceImpactEventData{
			.LocationOverride = FVector::ZeroVector,
			.VFXScale = VFXScale,
			.VolumeMultiplier = VolumeMultiplier,
			.PitchMultiplier = PitchMultiplier,
			.bFollow = bFollow,
			.bDisableSound = bDisableSound,
			.bDisableVisual = bDisableVisual,
			.bUseLocationOverride = false
		};
		BaseCharacter->OnCharacterSurfaceImpact(ImpactSocketName, ImpactType, ImpactData);
	}
}
