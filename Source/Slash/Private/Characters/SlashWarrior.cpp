// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashWarrior.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/AttackComboDataAsset.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/HealthObject.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

ASlashWarrior::ASlashWarrior()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	DesiredActionDirection = FVector(0.f);
}


void ASlashWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDesiredActionDirection();
	UpdateFacingActionDirection(DeltaTime);

	if (Attributes)
	{
		Attributes->RegenStamina(DeltaTime);
	}
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashWarrior::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* ThisController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhacedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer());
		EnhacedInputSubsystem->AddMappingContext(DefaultSlashContext, 0);

	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		//AnimInstance->OnMontageEnded.AddDynamic(this, &ASlashWarrior::OnMontageEnded);
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ASlashWarrior::OnMontageBlendingOut);
	}

	Tags.Add(FName("EngageableTarget"));

	GetCharacterMovement()->MaxWalkSpeed = UnequippedWalkSpeed;

	InitializeSlashOverlay();

}


void ASlashWarrior::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.0f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InputConfig)
		{
			EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Move);
			EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Look);
			EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Jump);
			EnhancedInputComponent->BindAction(InputConfig->EquipAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Equip);
			EnhancedInputComponent->BindAction(InputConfig->AttackAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Attack);
			EnhancedInputComponent->BindAction(InputConfig->DodgeAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Dodge);
			EnhancedInputComponent->BindAction(InputConfig->DanceAction, ETriggerEvent::Triggered, this, &ASlashWarrior::Dance);
		}
	}

}

float ASlashWarrior::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();

	return DamageAmount;
}

void ASlashWarrior::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashWarrior::IsMoving() const
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		return MovementComp->Velocity.Size() > 0.f;
	}
	return false;
}

void ASlashWarrior::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	OnComboWindowClosed();
}

void ASlashWarrior::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashWarrior::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashWarrior::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashWarrior::Heal(AHealthObject* HealthObject)
{
	if (Attributes)
	{
		Attributes->Heal(HealthObject->GetHealAmount());
	}
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashWarrior::CanHeal()
{
	if (Attributes)
	{
		return Attributes->GetHealthPercent() < 1.f;
	}
	return false;
}

void ASlashWarrior::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_EquippingOnRun) return;

	// This is fine because we are in Unoccupied state 
	CancelAttackMontage();
	CancelEquipMontage();

	FVector2D Direction = Value.Get<FVector2D>();
	if (!Direction.IsNearlyZero())
	{
		FVector2D NormalizedInput = Direction.GetSafeNormal();

		const float InputMagnitude = FMath::Clamp(Direction.Size(), 0.0f, 1.0f);

		const FRotator Rotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector Forward = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		const FVector MoveDirection = (Forward * NormalizedInput.Y + Right * NormalizedInput.X).GetSafeNormal();
		AddMovementInput(MoveDirection, InputMagnitude);
	}
}

void ASlashWarrior::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();
	if (LookAxis.SizeSquared() > 0.0f)
	{
		AddControllerPitchInput(LookAxis.Y);
		AddControllerYawInput(LookAxis.X);
	}
}

void ASlashWarrior::Jump()
{
	if (IsUnoccupied())
	{
		ACharacter::Jump();
	}
}

bool ASlashWarrior::IsUnoccupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashWarrior::IsAttacking() const
{
	return ActionState == EActionState::EAS_Attacking;
}

bool ASlashWarrior::IsDodgeAllowed() const
{
	return bDodgeAfterHitAllowed;
}

bool ASlashWarrior::IsHitReacting() const
{
	return ActionState == EActionState::EAS_HitReaction;
}

void ASlashWarrior::Equip()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ASlashWarrior::Attack()
{
	if (CanAttack())
	{
		PerformAttack();
	}
	else
	{
		float InputBufferTimeExtended = InputBufferTime;
		if (ActionState == EActionState::EAS_Dodge)
		{
			InputBufferTimeExtended = InputBufferTime + 0.2;
		}
		bAttackBuffered = true;
		if (bWantsToMove)
		{
			bBufferedFacing = true;
		}

		GetWorldTimerManager().ClearTimer(InputBufferTimer);
		GetWorldTimerManager().SetTimer(InputBufferTimer, this,
			&ASlashWarrior::ClearBufferedInput, InputBufferTimeExtended, false);
	}
}

void ASlashWarrior::Dodge()
{
	if (!HasEnoughStamina()) return;
	if (!IsUnoccupied() && !IsAttacking() && !IsHitReacting()) return;
	if (IsHitReacting() && !bDodgeAfterHitAllowed) return;

	bDodgeAfterHitAllowed = false;

	CancelAttackMontage();
	FaceDesiredActionDirection(RotationInterpSpeedDodge);
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
	}
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashWarrior::FaceDesiredActionDirection(float RotationSpeed)
{
	if ((bWantsToMove || bBufferedFacing) && !ActionDirectionIntent.IsNearlyZero())
	{
		bUseDesiredFacing = true;
		RotationInterpSpeed = RotationSpeed;
		DesiredActionDirection = ActionDirectionIntent;
	}
}

void ASlashWarrior::UpdateFacingActionDirection(float DeltaTime)
{
	if (bUseDesiredFacing && !DesiredActionDirection.IsNearlyZero())
	{
		FRotator TargetRot = DesiredActionDirection.Rotation();
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;

		FRotator CurrentRot = GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationInterpSpeed);

		bool bOrientToMovementSave = GetCharacterMovement()->bOrientRotationToMovement;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		SetActorRotation(NewRot);
		GetCharacterMovement()->bOrientRotationToMovement = bOrientToMovementSave;

		if (CurrentRot.Equals(TargetRot, 1.f))
		{
			bUseDesiredFacing = false;
		}
	}
}

void ASlashWarrior::UpdateDesiredActionDirection()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		ULocalPlayer* LP = PC->GetLocalPlayer();
		if (LP)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				const FInputActionValue Value = Subsystem->GetPlayerInput()->GetActionValue(InputConfig->MoveAction);

				FVector2D InputVector = Value.Get<FVector2D>();
				bWantsToMove = !InputVector.IsNearlyZero();

				if (bWantsToMove)
				{
					const FRotator Rotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
					const FVector Forward = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
					const FVector Right = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

					ActionDirectionIntent = (Forward * InputVector.Y + Right * InputVector.X).GetSafeNormal();
				}
			}
		}
	}
}

bool ASlashWarrior::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void ASlashWarrior::FindCombatTarget()
{
	if (EquippedWeapon == nullptr) return;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(EquippedWeapon);
	QueryParams.bFindInitialOverlaps = true;

	FVector Direction = ActionDirectionIntent.IsNearlyZero() ? GetActorForwardVector() : ActionDirectionIntent;

	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = SweepStart + Direction * CombatTargetSearchLength;

	TArray<FHitResult> OutHits;
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		SweepStart,
		SweepEnd,
		GetActorQuat(),
		ECC_Visibility,
		FCollisionShape::MakeSphere(CombatTargetSearchRadius),
		QueryParams
	);

	FHitResult ChosenResult;
	if (bHit)
	{
		for (FHitResult& Result : OutHits)
		{
			if (Result.GetActor()->ActorHasTag(TEXT("Enemy")))
			{
				CombatTarget = Result.GetActor();
				ChosenResult = Result;
				break;
			}
		}
	}
	/*
	FColor SColor = FColor::Cyan;
	if (bHit && ChosenResult.GetActor() && ChosenResult.GetActor()->ActorHasTag(TEXT("Enemy")))
	{
		SColor = FColor::Red;
	}
	DrawDebugSphere(GetWorld(), SweepStart, CombatTargetSearchRadius, 12, FColor::Green, false, 5.f);
	DrawDebugSphere(GetWorld(), SweepEnd, CombatTargetSearchRadius, 12, SColor, false, 5.f);
	*/
}

void ASlashWarrior::Dance()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (DanceMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(DanceMontage);
		AnimInstance->Montage_JumpToSection(FName("Dance"), DanceMontage);
	}
}

void ASlashWarrior::PerformAttack()
{
	FComboAttack SelectedAttack;
	if (SelectAttack(SelectedAttackData, SelectedAttack))
	{
		WarpTargetDistanceMax = SelectedAttack.AttackEffectiveRangeMax;
		WarpTargetDistanceMin = SelectedAttack.AttackEffectiveRangeMin;
		FaceDesiredActionDirection(RotationInterpSpeedAttack);
		FindCombatTarget();
		PlayMontageSection(SelectedAttackData->AttackMontage, SelectedAttack.MontageSection);
		ActionState = EActionState::EAS_Attacking;
		EquippedWeapon->SetDamageMultiplier(SelectedAttack.DamageMultiplier);
	}
}


void ASlashWarrior::PlayEquipMontage(const FName& SectionName)
{
	UAnimMontage* MontageToPlay = nullptr;
	if (GetMovementComponent() && GetMovementComponent()->Velocity.Size() > 1.f)
	{
		PlayMontageSection(SelectedEquipMontageUB, SectionName);
	}
	else
	{
		PlayMontageSection(SelectedEquipMontage, SectionName);
	}
}

void ASlashWarrior::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void ASlashWarrior::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		bComboWindowOpen = false;
		bAttackBuffered = false;
	}
}

void ASlashWarrior::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		bComboWindowOpen = false;
		bAttackBuffered = false;
	}
}

void ASlashWarrior::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashWarrior::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
	OnComboWindowOpened();
	//OnComboWindowClosed();
}

void ASlashWarrior::AllowDodgeAfterHitReact()
{
	Super::AllowDodgeAfterHitReact();
	bDodgeAfterHitAllowed = true;
}

bool ASlashWarrior::CanAttack() const
{
	return (ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped) || bComboWindowOpen;
}

bool ASlashWarrior::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashWarrior::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon != nullptr;
}

void ASlashWarrior::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	if (IsMoving())
	{
		ActionState = EActionState::EAS_EquippingOnRun;
	}
	else
	{
		ActionState = EActionState::EAS_Equipping;
	}
}

void ASlashWarrior::Arm()
{
	PlayEquipMontage(FName("Equip"));
	if (IsMoving())
	{
		ActionState = EActionState::EAS_EquippingOnRun;
	}
	else
	{
		ActionState = EActionState::EAS_Equipping;
	}
}

void ASlashWarrior::EquipWeapon(AWeapon* Weapon)
{
	AItem* OldWeapon = nullptr;
	if (EquippedWeapon)
	{
		EquippedWeapon->SwitchWith(Weapon);
		CharacterState = ECharacterState::ECS_Unequipped;
		OldWeapon = EquippedWeapon;
	}
	Weapon->Equip(GetMesh(), FName("hand_rSocket"), this, this);
	EquippedWeapon = Weapon;
	FinishEquipping();
	SetOverlappingItem(OldWeapon);

	switch (EquippedWeapon->GetWeaponType())
	{
	case ECharacterState::ECS_EquippedTwoHandedWeapon:
		SelectedEquipMontage = EquipTwoHandedMontage;
		SelectedEquipMontageUB = EquipTwoHandedMontageUB;
		SelectedAttackData = AttacksTwoHandedData;
		break;
	case ECharacterState::ECS_EquippedOneHandedWeapon:
	default:
		SelectedEquipMontage = EquipOneHandedMontage;
		SelectedEquipMontageUB = EquipOneHandedMontageUB;
		SelectedAttackData = AttacksOneHandedData;
		break;
	}
}

void ASlashWarrior::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("spine_05Socket"));
	}
}

void ASlashWarrior::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("hand_rSocket"));
	}
}

void ASlashWarrior::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
	CharacterState = EquippedWeapon->GetWeaponType();

	ChangeWalkSpeedOnCharacterState();

}

void ASlashWarrior::ChangeWalkSpeedOnCharacterState()
{
	float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	switch (CharacterState)
	{
	case ECharacterState::ECS_EquippedOneHandedWeapon:
		MaxWalkSpeed = EquippedOneHandedWalkSpeed;
		break;
	case ECharacterState::ECS_EquippedTwoHandedWeapon:
		MaxWalkSpeed = EquippedTwoHandedWalkSpeed;
		break;
	case ECharacterState::ECS_Unequipped:
	default:
		MaxWalkSpeed = UnequippedWalkSpeed;
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ASlashWarrior::FinishUnequipping()
{
	ActionState = EActionState::EAS_Unoccupied;
	CharacterState = ECharacterState::ECS_Unequipped;
	ChangeWalkSpeedOnCharacterState();
}

void ASlashWarrior::OnComboWindowOpened()
{
	if (bAttackBuffered)
	{
		bAttackBuffered = false;
		GetWorldTimerManager().ClearTimer(InputBufferTimer);

		PerformAttack();
	}
	bComboWindowOpen = true;
	bBufferedFacing = false;
}

void ASlashWarrior::OnComboWindowClosed()
{
	if (SelectedAttackData)
	{
		SelectedAttackData->CurrentAttackIndex = 0;
	}
	bComboWindowOpen = false;
}

void ASlashWarrior::OnHitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashWarrior::ClearBufferedInput()
{
	bAttackBuffered = false;
}

void ASlashWarrior::CancelAttackMontage()
{
	Super::CancelAttackMontage();
	CombatTarget = nullptr;
	if (SelectedAttackData)
	{
		SelectedAttackData->CurrentAttackIndex = 0;
	}
}

void ASlashWarrior::CancelEquipMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;
	if (AnimInstance->GetCurrentActiveMontage() == nullptr) return;

	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();

	if (CurrentMontage == SelectedEquipMontage)
	{
		AnimInstance->Montage_Stop(0.25, CurrentMontage);
	}
}
