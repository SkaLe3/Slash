// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MetasoundSource.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CollisionChannels.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive())
	{
		if (Hitter)
		{
			FName SectionName;
			DetermineHitDirectionForSectionName(SectionName, Hitter->GetActorLocation());
			PlayHitReactMontage(SectionName);
		}
	}
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::PerformAttack()
{

}

bool ABaseCharacter::SelectAttack(UAttackComboDataAsset* AttacksData, FComboAttack& OutAttack)
{
	if (AttacksData == nullptr || AttacksData->ComboAttacks.Num() <= 0) return false;

	if (AttacksData->CurrentAttackIndex >= AttacksData->ComboAttacks.Num() || AttacksData->CurrentAttackIndex < 0)
	{
		AttacksData->CurrentAttackIndex = 0;
	}

	OutAttack = AttacksData->ComboAttacks[AttacksData->CurrentAttackIndex];
	OutAttack.Montage = AttacksData->AttackMontage;

	if (OutAttack.NextAttackIndices.Num() > 0)
	{
		const int32 NextAttackIndicesIndex = FMath::RandRange(0, OutAttack.NextAttackIndices.Num() - 1);
		const int32 NextAttackIndex = OutAttack.NextAttackIndices[NextAttackIndicesIndex];

		AttacksData->CurrentAttackIndex = NextAttackIndex;
	}
	return true;
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::CanAttack() const
{
	return false;
}

bool ABaseCharacter::IsAlive() const
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::DodgeEnd()
{

}

void ABaseCharacter::AllowDodgeAfterHitReact()
{

}

void ABaseCharacter::PlayMontageSection(UAnimMontage* MontageToPlay, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (MontageToPlay && AnimInstance)
	{
		AnimInstance->Montage_Play(MontageToPlay);
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}
}

int32 ABaseCharacter::PlayRandomMotageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMotageSection(DeathMontage, DeathMontageSections);
	EDeathPose Pose = static_cast<EDeathPose>(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::CancelAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance->GetCurrentActiveMontage() == nullptr) return;
	if (AnimInstance && SelectedAttackData && AnimInstance->GetCurrentActiveMontage() == SelectedAttackData->AttackMontage)
	{
		AnimInstance->Montage_Stop(0.25, SelectedAttackData->AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = Location - CombatTargetLocation;
	FVector TargetToMeNormalized = TargetToMe.GetSafeNormal();

	// Keeps the warp distance within a safe range:
	// - If the target is closer than the max allowed distance, use the real distance (no overshoot).
	// - If the target is too close, enforce a minimum offset so the character doesn't clip
	FVector AttackDistance = TargetToMeNormalized * FMath::Max(FMath::Min(WarpTargetDistanceMax, TargetToMe.Size()), WarpTargetDistanceMin);

	return CombatTargetLocation + AttackDistance;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DetermineHitDirectionForSectionName(FName& OutName, const FVector& ImpactPoint)
{

	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();
	const FVector EnemyLocation = GetMesh()->GetComponentLocation();

	FVector HitDirectionXYZ = EnemyLocation - ImpactPoint;
	HitDirectionXYZ.Z = 0.f;
	const FVector HitDirection = (HitDirectionXYZ).GetSafeNormal();

	const double DotProduct = FVector::DotProduct(ForwardVector, HitDirection);
	const double Threshold = FMath::Cos(FMath::DegreesToRadians(45.f));

	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, HitDirection);
	bool SideRightFlag = false;
	if (CrossProduct.Z < 0)
	{
		SideRightFlag = true;
	}

	OutName = OutName = FName("FromLeft");
	if (DotProduct > Threshold)
	{
		OutName = FName("FromBack");
	}
	else if (DotProduct < -Threshold)
	{
		OutName = FName("FromFront");
	}
	else if (SideRightFlag)
	{
		OutName = FName("FromRight");
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::OnCharacterSurfaceImpact(FName ImpactSocketName, EImpactType ImpactType, const FSurfaceImpactEventData& ImpactProperties)
{
	if (!ImpactProperties.bUseLocationOverride && !GetMesh()->DoesSocketExist(ImpactSocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCharacterSurfaceImpact: Socket '%s' does not exist. Using component location"), *ImpactSocketName.ToString());
	}

	FVector ImpactLocation;
	if (ImpactProperties.bUseLocationOverride)
	{
		ImpactLocation = ImpactProperties.LocationOverride;
	}
	else
	{
		ImpactLocation = GetMesh()->GetSocketLocation(ImpactSocketName);
	}

	FHitResult HitResult;
	FVector Start = ImpactLocation + FVector(0.f, 0.f, 5.f);
	FVector End = ImpactLocation - FVector(0.f, 0.f, 50.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bReturnPhysicalMaterial = true;

	ECollisionChannel SurfaceImpactTraceChannel = SurfaceImpact_TraceChannel;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, SurfaceImpactTraceChannel, QueryParams);
	if (bHit)
	{
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
		if (SurfaceImpactDataAsset)
		{
			if (!ImpactProperties.bDisableSound)
			{
				USoundBase* ImpactSound = SurfaceImpactDataAsset->GetSound(SurfaceType, ImpactType);
				if (ImpactSound)
				{
					UMetaSoundSource* MetaSound = Cast<UMetaSoundSource>(ImpactSound);
					if (MetaSound)
					{
						if (ImpactProperties.bFollow)
						{
							UGameplayStatics::SpawnSoundAttached(MetaSound, GetMesh(), ImpactSocketName, FVector::ZeroVector, EAttachLocation::SnapToTarget, true, ImpactProperties.VolumeMultiplier, ImpactProperties.PitchMultiplier);
						}
						else
						{
							UGameplayStatics::SpawnSoundAtLocation(this, MetaSound, ImpactLocation, FRotator::ZeroRotator, ImpactProperties.VolumeMultiplier, ImpactProperties.PitchMultiplier);
						}
					}
					else
					{
						UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactLocation, ImpactProperties.VolumeMultiplier, ImpactProperties.PitchMultiplier);
					}
				}
			}
			if (!ImpactProperties.bDisableVisual)
			{
				UNiagaraSystem* ImpactVisual = SurfaceImpactDataAsset->GetVisual(SurfaceType, ImpactType);
				if (ImpactVisual)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactVisual, ImpactLocation, FRotator::ZeroRotator, FVector(ImpactProperties.VFXScale));
				}
			}

		}
	}
	/*
	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 10.f);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5, 12, FColor::Red, false, 10.f);
	}
	*/
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->ClearHitEnemiesThisAttack();

	}
}

void ABaseCharacter::LoseCombatTarget()
{
	CombatTarget = nullptr;
}

