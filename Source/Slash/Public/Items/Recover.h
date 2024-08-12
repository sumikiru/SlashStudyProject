// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Recover.generated.h"

UCLASS()
class SLASH_API ARecover : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:
	UPROPERTY(EditAnywhere,Category = "Recover Properties")
	float RecoverAmount = 25.f;
public:
	FORCEINLINE float GetRecoverAmount() const { return RecoverAmount; }
	FORCEINLINE void SetRecoverAmount(float Amount) { RecoverAmount = Amount; }
	//不在这里SetHealth

};
