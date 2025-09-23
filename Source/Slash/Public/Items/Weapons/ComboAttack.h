

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ComboAttack.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FComboAttack
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName MontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TArray<int32> NextAttackIndices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackEffectiveRangeMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackEffectiveRangeMin = 100.f;

	UAnimMontage* Montage = nullptr;

};
