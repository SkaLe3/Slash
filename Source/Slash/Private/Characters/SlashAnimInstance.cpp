// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashWarrior.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashWarrior>(TryGetPawnOwner());
	SlashCharacterMovement = SlashCharacter ? SlashCharacter->GetCharacterMovement() : nullptr;
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		Velocity = SlashCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);
		bIsFalling = SlashCharacterMovement->IsFalling();
		bShouldMove = GroundSpeed > 3.0f && SlashCharacterMovement->GetCurrentAcceleration() != FVector::ZeroVector;
		CharacterState = SlashCharacter ? SlashCharacter->GetCharacterState() : ECharacterState::ECS_Unequipped;
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPose();
	}
}
