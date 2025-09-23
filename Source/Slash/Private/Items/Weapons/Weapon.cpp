// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashWarrior.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(ItemMesh);

	BladeStart = CreateDefaultSubobject<USceneComponent>(TEXT("BladeStart"));
	BladeStart->SetupAttachment(ItemMesh);
	BladeEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BladeEnd"));
	BladeEnd->SetupAttachment(ItemMesh);

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	ItemMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PlayEquipSound();
	DeactivateEmbers();

}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	Sphere->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::SwitchWith(AWeapon* NewWeapon)
{
	Sphere->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemState = EItemState::EIS_Hovering;
	if (ItemEffect)
	{
		ItemEffect->Activate();
	}
	SetActorLocation(NewWeapon->GetActorLocation());
	SetActorRotation(NewWeapon->GetActorRotation());
}

void AWeapon::SetDamageMultiplier(float InDamageMultiplier)
{
	DamageMultiplier = InDamageMultiplier;
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	TArray<FHitResult> Results;
	const bool bHit = BoxTrace(Results);
	if (bHit)
	{
		for (auto& HitResult : Results)
		{
			if (HitResult.GetActor() && !HasHitEnemy(HitResult.GetActor()))
			{
				if (ActorIsSameType(HitResult.GetActor())) return;
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage * DamageMultiplier, GetInstigator()->GetController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				CreateFields(HitResult.ImpactPoint);
			}
		}
	}
#if 0
	if (bHit)
	{
		Debug_DrawSwordTrace(SweepStart, SweepEnd);
	}
#endif
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& Result)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(Result.GetActor());
	if (HitInterface)
	{
		AddHitEnemy(Result.GetActor());
		HitInterface->Execute_GetHit(Result.GetActor(), Result.ImpactPoint, GetOwner());
	}
}

bool AWeapon::BoxTrace(TArray<FHitResult>& BoxHits)
{
	if (!BladeStart || !BladeEnd) return false;

	const FVector SweepStart = BladeStart->GetComponentLocation();
	const FVector SweepEnd = BladeEnd->GetComponentLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepMultiByChannel(
		BoxHits,
		SweepStart,
		SweepEnd,
		WeaponBox->GetComponentQuat(),
		ECC_Visibility,
		FCollisionShape::MakeBox(FVector3d(WeaponBox->GetUnscaledBoxExtent().X, WeaponBox->GetUnscaledBoxExtent().Y, 5.f)),
		QueryParams
	);
	return bHit;
}

#if WITH_EDITOR
void AWeapon::Debug_DrawSwordTrace(const FVector& Start, const FVector& End)
{
	// Visualize the sweep path
	DrawDebugBox(
		GetWorld(),
		Start,
		FVector(4.6f, 4.6f, 4.6f),
		WeaponBox->GetComponentQuat(),
		FColor::Green,
		false,
		5.f
	);

	DrawDebugBox(
		GetWorld(),
		End,
		FVector(4.6f, 4.6f, 4.6f),
		WeaponBox->GetComponentQuat(),
		FColor::Blue,
		false,
		5.f
	);

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Cyan,
		false,
		5.f,
		0,
		2.f
	);
}
#endif
