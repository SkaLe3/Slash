// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Items/Weapons/AttackComboDataAsset.h"
#include "Characters/CharacterTypes.h"
#include "Animation/SurfaceImpactDataAsset.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMotage;
class UAttackComboDataAsset;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	UFUNCTION(BlueprintCallable)
	void OnCharacterSurfaceImpact(FName ImpactSocketName, EImpactType ImpactType, const FSurfaceImpactEventData& ImpactProperties);

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

	virtual void Attack();
	virtual void PerformAttack();
	virtual bool CanAttack() const;

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	virtual void HandleDamage(float DamageAmount);
	void DetermineHitDirectionForSectionName(FName& OutName, const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	bool IsAlive() const;
	bool SelectAttack(UAttackComboDataAsset* AttacksData, FComboAttack& OutAttack);
	void DisableMeshCollision();

	void PlayMontageSection(UAnimMontage* MontageToPlay, const FName& SectionName);
	int32 PlayRandomMotageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	virtual void CancelAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	virtual void AllowDodgeAfterHitReact();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void LoseCombatTarget();

public:
	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }

protected:
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	USurfaceImpactDataAsset* SurfaceImpactDataAsset;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAttackComboDataAsset* AttacksOneHandedData;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAttackComboDataAsset* AttacksTwoHandedData;

	UAttackComboDataAsset* SelectedAttackData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistanceMax = 75.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistanceMin = 30.f;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;
};
