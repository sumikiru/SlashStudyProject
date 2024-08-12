// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"

void ASoul::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const double LocationZ = GetActorLocation().Z;
	if(LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f,0.f,DriftRate * DeltaSeconds);
		AddActorWorldOffset(DeltaLocation);
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	// const FVector Start = GetActorLocation();
	// const FVector End = Start - FVector(0.f,0.f,2000.f);
	//
	// TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	// TArray<AActor*> ActorsToIgnore;
	// ActorsToIgnore.Add(GetOwner());//已经SetOwner(this)了
	 FHitResult HitResult;
	
	// UKismetSystemLibrary::LineTraceSingleForObjects(
	// 	this,
	// 	Start,
	// 	End,
	// 	ObjectTypes,
	// 	false,
	// 	ActorsToIgnore,
	// 	EDrawDebugTrace::ForDuration,
	// 	HitResult,
	// 	true,
	// 	FLinearColor::Red,
	// 	FLinearColor::Green,
	// 	10.f
	// );

	DesiredZ = HitResult.ImpactPoint.Z + 125.f;
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->ActorHasTag("EngagedTarget"))
	{
		return;
	}
	IPickupInterface* PickupInterface=Cast<IPickupInterface>(OtherActor);
	if(PickupInterface)
	{
		Souls = FMath::RandRange(1, 5);//每次拾取都是随机的
		PickupInterface->AddSouls(this);
		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();
	}
}
