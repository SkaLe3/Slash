// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "HealthObject.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AHealthObject : public AItem
{
	GENERATED_BODY()
	
public:
	FORCEINLINE float GetHealAmount() const { return HealAmount; }
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Heal Properties")
	float HealAmount;
};
