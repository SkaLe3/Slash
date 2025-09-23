// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	ASoul();
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	void LaunchUp();

	void CheckGround();

public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual void OnPullSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnPullSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* PullSphere;

	UPROPERTY(VisibleAnywhere, Category = "Soul Properties | Pickup");
	AActor* PullTarget;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	float PullForceStrength = 2000.f; // Acceleration units per second^2

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	float ForceDamping = 0.98f; 

	UPROPERTY(VisibleAnywhere, Category = "Soul Properties | Pickup")
	FVector PullVelocity;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Spawn")
	float HoverDistanceAboveGround = 50.f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Spawn")
	float DescendingSpeed = 50.f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Spawn")
	float InitialVerticalSpeed = 2000.f;

	UPROPERTY(VisibleAnywhere, Category = "Soul Properties | Spawn")
	float DesiredWorldHeight;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	bool bHorizontalPullOnly = false;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	bool bCanEverPickup = true;

	UPROPERTY(EditAnywhere, Category = "Soul Properties | Pickup")
	float TimeToPickupEnabled = 1.f;
};
