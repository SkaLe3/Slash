// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Characters/CharacterTypes.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

/**
 *
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void DeactivateEmbers();

	void DisableSphereCollision();

	void PlayEquipSound();

	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void SwitchWith(AWeapon* NewWeapon);

	void ClearHitEnemiesThisAttack() { HitEnemiesThisAttack.Empty(); }
	void AddHitEnemy(AActor* HitEnemy) { HitEnemiesThisAttack.Add(HitEnemy); }
	bool HasHitEnemy(AActor* HitEnemy) { return HitEnemiesThisAttack.Contains(HitEnemy); }

	void SetDamageMultiplier(float InDamageMultiplier);

protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& Result);


	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	bool BoxTrace(TArray<FHitResult>& BoxHits);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BladeStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BladeEnd;

	TSet<AActor*> HitEnemiesThisAttack;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	float DamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	ECharacterState WeaponType = ECharacterState::ECS_EquippedOneHandedWeapon;



private:
#if WITH_EDITOR
	void Debug_DrawSwordTrace(const FVector& Start, const FVector& End);
#endif
public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE ECharacterState GetWeaponType() const { return WeaponType; }
};
