// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	/** <ABaseActor> */
	virtual void Tick(float DeltaTime) override;
	/** </ABaseActor> */

protected:
	virtual void BeginPlay() override;

	/** <Combat Effects> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual bool CanAttack();
	virtual void Attack();
	bool IsAlive();
	UFUNCTION(BlueprintNativeEvent)
	void Die();//现在的函数实现应该为Die_Implementation();//不需要再写成之前的virtual void Die();
	
	void DisableMeshCollision();
	void DisableCapsule();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	//Play Specific Montage
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);//所有不需要重写的（也包括EquippedWeapon等等，都不需要再在SlashCharacter/Enemy...中写了）
	/** </Combat Effects> */
	
	UPROPERTY(VisibleAnywhere,Category="Weapon")
	AWeapon* EquippedWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	/** <Play Montage Section> */
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	/** </Play Montage Section> */
	
	/** <Sound && Particles> */
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;
	//Cascade粒子系统（旧）
	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;
	/** </Sound && Particles> */

	/** <Anim Montages> */
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName>AttackMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName>DeathMontageSections;
	/** </Anim Montages> */

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
