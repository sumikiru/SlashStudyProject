// Fill out your copyright notice in the Description page of Project Settings.

#include "Breakable/BreakableActor.h"
#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"

ABreakableActor::ABreakableActor()
{
 	PrimaryActorTick.bCanEverTick = false;//注意

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	// Capsule->SetCapsuleHalfHeight(95.f);
	// Capsule->SetCapsuleRadius(60.f);
	// Capsule->SetRelativeLocation({0.f,0.f,78.f});
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	//GeometryCollection->OnChaosBreakEvent.AddDynamic(this,&ABreakableActor::SetLifeSpan(2.f));
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(bBroken)
	{
		return;
	}
	bBroken = true;
	UWorld* World = GetWorld();
	if(World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		const int32 Selection = FMath::RandRange(0,TreasureClasses.Num() - 1);
		World->SpawnActor<ATreasure>(
			TreasureClasses[Selection],//小心数组越界
			Location,
			GetActorRotation()
		);
	}
}

