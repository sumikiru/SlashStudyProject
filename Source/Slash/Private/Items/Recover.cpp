// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Recover.h"
#include "Interfaces/PickupInterface.h"

void ARecover::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->ActorHasTag("EngagedTarget"))
	{
		return;
	}
	IPickupInterface* PickupInterface=Cast<IPickupInterface>(OtherActor);
	if(PickupInterface)
	{
		PickupInterface->AddHealth(this);
		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();
	}
}
