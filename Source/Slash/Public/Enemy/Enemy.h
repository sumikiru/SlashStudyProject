// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class ARecover;
class ASoul;
class UWidgetComponent;
class UHealthBarComponent;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter //别忘了修改成public ABaseCharacter而不是ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	void SpawnRecover();
	virtual void Attack() override;
	bool bIsSeeingPlayer = false;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	//virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;//记得AM_Attack和ABP_Paladin的事件图表里面也要用
	/** </ABaseCharacter> */
	
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/** <Getter and Setter>*/
	/** </Getter and Setter>*/

private:
	/** <AI Behaviour> */
	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent
	/** </AI Behaviour> */

	/** <Components> */
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> WeaponClass;
	/** </Components> */

	/** <AI Behaviour Variables>*/
	//combat && attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 40.f;
	UPROPERTY()
	class AAIController* EnemyController;
	//Patrol
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")//, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")在C++不需要
	AActor* PatrolTarget;//patrol:巡逻
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*>PatrolTargets;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 400.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;
	double PatrolWaitTime = FMath::RandRange(PatrolWaitMin,PatrolWaitMax);//这在游戏开始前就固定了
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;
	FTimerHandle PatrolTimer;
	//Attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.25f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;
	FTimerHandle AttackTimer;
	//Death
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 3.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ASoul> SoulClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ARecover> RecoverClass;
	/** </AI Behaviour Variables>*/
};
