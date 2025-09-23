// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Items/Weapons/ComboAttack.h"
#include "AttackComboDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SLASH_API UAttackComboDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<FComboAttack> ComboAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentAttackIndex = 0;
};
