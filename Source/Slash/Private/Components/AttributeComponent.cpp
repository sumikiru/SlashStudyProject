// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;//已经有了RegenStamina(float DeltaTime)
	
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.0f, MaxStamina);
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	SoulCount += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	GoldCount += AmountOfGold;
}

void UAttributeComponent::SetGoldCount(int32 AmountOfGold)
{
	GoldCount = AmountOfGold;
}

void UAttributeComponent::AddHealth(float AmountOfHealth)
{
	Health = FMath::Clamp(Health + AmountOfHealth, 0.0f, MaxHealth);
}

void UAttributeComponent::RecoverHealthToMax()
{
	Health = MaxHealth;
}

void UAttributeComponent::Upgrade()
{
	MaxHealth += 20;
	Health = MaxHealth;
	MaxStamina += 20.f;
	Stamina = MaxStamina;
	StaminaRegenRate += 1.f;
	DodgeStaminaCost = FMath::Clamp(DodgeStaminaCost - 1.f, 5.f, 20.f);
	UnoccupiedStaminaRegenRate += 1.f;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	if(!IsAlive()) return;
	
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
}

