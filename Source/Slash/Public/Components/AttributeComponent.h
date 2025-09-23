// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
protected:
	virtual void BeginPlay() override;


public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent() const;
	float GetStaminaPercent() const;
	bool IsAlive() const;
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	void Heal(float HealAmount);


	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }

private:
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float StaminaRegenRate = 8.f;
};
