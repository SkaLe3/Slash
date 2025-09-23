// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurfaceImpactDescription.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EImpactType : uint8
{
	EIT_Walk UMETA(DisplayName = "Walk"),
	EIT_Run UMETA(DisplayName = "Run"),
	EIT_Jump UMETA(DisplayName = "Jump"),
	EIT_Land UMETA(DisplayName = "Land"),
	EIT_Fall UMETA(DisplayName = "Fall"),
	EIT_HitLight UMETA(DisplayName = "HitLight"),
	EIT_HitHeavy UMETA(DisplayName = "HitHeavy")
};

/**
 * 
 */
UCLASS()
class SLASH_API USurfaceImpactDescription : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere)
	EImpactType ImpactType;

	UPROPERTY(EditAnywhere)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* Vfx;
};
