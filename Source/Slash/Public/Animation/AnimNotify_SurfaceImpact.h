// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/SurfaceImpactDataAsset.h"
#include "AnimNotify_SurfaceImpact.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UAnimNotify_SurfaceImpact : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	EImpactType ImpactType = EImpactType::EIT_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	FName ImpactSocketName = "Footstep_R";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	double VFXScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact", meta = (ToolTip = "If this sound should follow its owner"))
	bool bFollow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	bool bDisableSound = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	bool bDisableVisual = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
