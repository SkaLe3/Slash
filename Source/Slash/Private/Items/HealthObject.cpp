// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/HealthObject.h"
#include "Interfaces/PickupInterface.h"

void AHealthObject::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		if (PickupInterface->CanHeal())
		{
			PickupInterface->Heal(this);
			SpawnPickupSystem();
			SpawnPickupSound();
			Destroy();
		}
	}
}
