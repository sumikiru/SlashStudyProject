// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	
protected:
	virtual void BeginPlay() override;

private:
	//current health
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float Health = 100.f;//当前生命值
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float MaxHealth = 100.f;
	//current stamina
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float StaminaRegenRate = 5.f;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float DodgeStaminaCost = 15.f;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float UnoccupiedStaminaRegenRate = 10.f;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	int32 GoldCount = 0;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	int32 SoulCount = 0;
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float RecoverAmount = 25.f;

public:
	//Getters && Setters
	FORCEINLINE float GetHealth() const {return Health;}
	FORCEINLINE float GetHealthPercent() const {return Health / MaxHealth;}
	FORCEINLINE float GetStaminaPercent() const {return Stamina / MaxStamina;}
	FORCEINLINE float GetStamina() const {return Stamina;}
	FORCEINLINE float GetDodgeStaminaCost() const {return DodgeStaminaCost;}
	FORCEINLINE void SetUnoccupiedStaminaRegenRate() { StaminaRegenRate = UnoccupiedStaminaRegenRate;}
	FORCEINLINE void SetDodgeStaminaRegenRate() {StaminaRegenRate = 0.f;}
	FORCEINLINE int32 GetGoldCount() const {return GoldCount;}
	FORCEINLINE int32 GetSoulCount() const {return SoulCount;}
	FORCEINLINE float GetRecoverCount() const {return RecoverAmount;}

	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	bool IsAlive() const;
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	void SetGoldCount(int32 AmountOfGold);
	void AddHealth(float AmountOfHealth);
	void RecoverHealthToMax();
	void Upgrade();
};
