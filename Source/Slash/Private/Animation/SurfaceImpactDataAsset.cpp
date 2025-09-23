// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SurfaceImpactDataAsset.h"
#include "NiagaraSystem.h"

USoundBase* USurfaceImpactDataAsset::GetSound(EPhysicalSurface Surface, EImpactType ImpactType) const
{
	for (const USurfaceImpactDescription* Entry : ImpactData)
	{
		if (Entry->SurfaceType == Surface && Entry->ImpactType == ImpactType)
		{
			return Entry->Sound;
		}
	}
	return DefaultSound;
}

UNiagaraSystem* USurfaceImpactDataAsset::GetVisual(EPhysicalSurface Surface, EImpactType ImpactType) const
{
	for (const USurfaceImpactDescription* Entry : ImpactData)
	{
		if (Entry->SurfaceType == Surface && Entry->ImpactType == ImpactType)
		{
			return Entry->Vfx;
		}
	}
	return DefaultVfx;
}
