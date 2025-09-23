// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PhysicsEngine/PhysicsSettings.h" 
#include "SurfaceImpactDescription.h"
#include "SurfaceImpactDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FSurfaceImpactEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FVector LocationOverride = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	double VFXScale = 1.f;
	UPROPERTY(BlueprintReadWrite)
	float VolumeMultiplier = 1.f;
	UPROPERTY(BlueprintReadWrite)
	float PitchMultiplier = 1.f;
	UPROPERTY(BlueprintReadWrite)
	bool bFollow = false;
	UPROPERTY(BlueprintReadWrite)
	bool bDisableSound = false;
	UPROPERTY(BlueprintReadWrite)
	bool bDisableVisual = false;
	UPROPERTY(BlueprintReadWrite)
	bool bUseLocationOverride = false;
};

/**
 * 
 */
UCLASS()
class SLASH_API USurfaceImpactDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<USurfaceImpactDescription*> ImpactData;

	UPROPERTY(EditAnywhere)
	USoundBase* DefaultSound;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DefaultVfx;

	USoundBase* GetSound(EPhysicalSurface Surface, EImpactType ImpactType) const;
	UNiagaraSystem* GetVisual(EPhysicalSurface Surface, EImpactType ImpactType) const;
};
