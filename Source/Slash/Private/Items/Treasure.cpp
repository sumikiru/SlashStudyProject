// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Treasure.h"
#include "Characters/SlashCharacter.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//不写Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if(!OtherActor->ActorHasTag("EngagedTarget"))
	{
		return;
	}
	IPickupInterface* PickupInterface=Cast<IPickupInterface>(OtherActor);
	if(PickupInterface)
	{
		PickupInterface->AddGold(this);
		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();
	}
}
