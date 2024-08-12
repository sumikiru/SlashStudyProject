// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class ARecover;
class ATreasure;
class ASoul;
class AItem;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOverlappingItem(AItem* Item);
	virtual void AddSouls(ASoul* Soul);
	virtual void AddGold(ATreasure* Gold);
	virtual void AddHealth(ARecover* Recover);
};
