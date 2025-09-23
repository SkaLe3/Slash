// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"

ASoul::ASoul()
{
	PrimaryActorTick.bCanEverTick = true;

	PullSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Pull Sphere"));
	PullSphere->SetupAttachment(GetRootComponent());
	PullSphere->SetSphereRadius(100.f);

	PullSphere->OnComponentBeginOverlap.AddDynamic(this, &ASoul::OnPullSphereOverlap);
	PullSphere->OnComponentEndOverlap.AddDynamic(this, &ASoul::OnPullSphereEndOverlap);
}

void ASoul::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckGround();

	FVector CurrentLocaiton = GetActorLocation();
	if (CurrentLocaiton.Z > DesiredWorldHeight)
	{
		AddActorWorldOffset(FVector(0, 0, -DescendingSpeed * DeltaSeconds));
	}

	FVector PullDirection = FVector(0.f, 0.f, 0.f);
	if (PullTarget)
	{
	
		PullDirection = (PullTarget->GetActorLocation() - GetActorLocation());
		if (bHorizontalPullOnly)
		{
			PullDirection.Z = 0;
		}
		PullDirection = PullDirection.GetSafeNormal();
	}

	PullVelocity += PullDirection * PullForceStrength * DeltaSeconds;
	PullVelocity *= ForceDamping;

	FVector NewLocation = GetActorLocation() + PullVelocity * DeltaSeconds;
	NewLocation.Z = FMath::Clamp(NewLocation.Z, DesiredWorldHeight, 100000000);
	SetActorLocation(NewLocation, true);


	//DrawDebugSphere(GetWorld(), GetActorLocation(), PullSphere->GetScaledSphereRadius(), 12, FColor::Emerald);
	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + PullVelocity.GetSafeNormal() * 100.f, 10.f, FColor::Green);

}

void ASoul::BeginPlay()
{
	Super::BeginPlay();
	LaunchUp();

	CheckGround();

}

void ASoul::LaunchUp()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PullVelocity = FVector(0.f, 0.f, InitialVerticalSpeed);
	FTimerHandle TimeToPickup_TimerHandle = FTimerHandle();
	GetWorldTimerManager().SetTimer(TimeToPickup_TimerHandle, [this]()
		{
			Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	, TimeToPickupEnabled, false);
}

void ASoul::CheckGround()
{
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart - FVector(0, 0, 1000.f);
	FCollisionObjectQueryParams QueryObjectParams;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(this);
	if (PullTarget) QueryParams.AddIgnoredActor(PullTarget);
	QueryObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	QueryObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);


	FHitResult Result;
	const bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Result,
		TraceStart,
		TraceEnd,
		QueryObjectParams,
		QueryParams
	);

	if (bHit)
	{
		FVector DestinationPoint = Result.ImpactPoint + FVector(0, 0, HoverDistanceAboveGround);
		DesiredWorldHeight = DestinationPoint.Z;
	}
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanEverPickup == false) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddSouls(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}
}

void ASoul::OnPullSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PullTarget = OtherActor;
	}
}

void ASoul::OnPullSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PullTarget == OtherActor)
	{
		PullTarget = nullptr;
	}

}
