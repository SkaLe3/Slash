// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputConfigData.h"
#include "CharacterTypes.h"
#include "Items/Weapons/ComboAttack.h"
#include "Interfaces/PickupInterface.h"
#include "SlashWarrior.generated.h"

class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul;
class ATreasure;
class USlashOverlay;

UCLASS()
class SLASH_API ASlashWarrior : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashWarrior();
	virtual void Tick(float DeltaTime) override;

	/** <AActor> */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

	/** <IPickupInterface> */
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void Heal(class AHealthObject* HealthObject) override;

	virtual bool CanHeal() override;
	/** </IPickupInterface> */
protected:
	virtual void BeginPlay() override;

	/** Callbacks for input*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump() override;
	void Equip();
	virtual void Attack() override;
	void Dodge();
	void Dance();



	/** Combat */
	virtual void PerformAttack() override;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual void AllowDodgeAfterHitReact() override;
	virtual bool CanAttack() const override;
	bool CanDisarm() const;
	bool CanArm() const;
	void Disarm();
	void Arm();
	void EquipWeapon(AWeapon* Weapon);
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	void FindCombatTarget();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	void ChangeWalkSpeedOnCharacterState();

	UFUNCTION(BlueprintCallable)
	void FinishUnequipping();

	UFUNCTION(BlueprintCallable)
	void OnComboWindowOpened();

	UFUNCTION(BlueprintCallable)
	void OnComboWindowClosed();

	UFUNCTION(BlueprintCallable)
	void OnHitReactEnd();

	UFUNCTION()
	void ClearBufferedInput();
private:
	virtual void CancelAttackMontage() override;
	virtual void CancelEquipMontage();
	bool IsUnoccupied() const;
	bool IsAttacking() const;
	bool IsDodgeAllowed() const;
	bool IsHitReacting() const;
	void FaceDesiredActionDirection(float RotationSpeed);
	void UpdateFacingActionDirection(float DeltaTime);
	void UpdateDesiredActionDirection();

	/** UI */
	void InitializeSlashOverlay();
	void SetHUDHealth();

	bool IsMoving() const;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultSlashContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputConfigData* InputConfig;

private:
	/** Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipOneHandedMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipOneHandedMontageUB;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipTwoHandedMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipTwoHandedMontageUB;

	UAnimMontage* SelectedEquipMontage;

	UAnimMontage* SelectedEquipMontageUB;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float InputBufferTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatTargetSearchRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatTargetSearchLength = 100.f;
	
	FTimerHandle InputBufferTimer;
	bool bAttackBuffered = false;
	bool bComboWindowOpen = false;
	bool bDodgeAfterHitAllowed = false;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float RotationInterpSpeedAttack = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float RotationInterpSpeedDodge = 10.f;

	float RotationInterpSpeed = 10.f;

	bool bWantsToMove = false;
	bool bUseDesiredFacing = false;
	bool bBufferedFacing = false;
	FVector DesiredActionDirection;
	FVector ActionDirectionIntent;

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY();
	USlashOverlay* SlashOverlay;


	UPROPERTY(EditAnywhere, Category = "Movement")
	float UnequippedWalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float EquippedOneHandedWalkSpeed = 480.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float EquippedTwoHandedWalkSpeed = 400.f;
	/**
	 * 
	 * Misc 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DanceMontage;
};
