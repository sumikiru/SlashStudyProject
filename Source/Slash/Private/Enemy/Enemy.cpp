// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Characters/SlashCharacter.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Recover.h"
#include "Items/Soul.h"
#include "Items/Weapons/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());

	//使角色朝向目标方向移动，而非控制器控制
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsDead()) return;
	if(EnemyState > EEnemyState::EES_Patrolling)//EES_Patrolling = 1
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if(IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if(IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	Super::Destroyed();

	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if(!IsDead()) ShowHealthBar();
	ClearPatrolTimer();//注意
	ClearAttackTimer();//注意
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);//假如被击中并且中断攻击动作，需要禁止武器碰撞以确保无误

	StopAttackMontage();
	
	if(IsInsideAttackRadius())
	{
		if(!IsDead())//确保死后不会起身攻击
		{
			StartAttackTimer();//主要针对Raptor，因为Raptor受击以后不会退出至范围外
		}
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if(PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this,&AEnemy::PawnSeen);
	}
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();//注意这里也要改
	
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();//!!!
	//PlayDeathMontage();//BaseCharacter.cpp已经添加
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;//确保留在原地而不会抽搐
	//GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);//避免在敌人死亡后武器还会击中SlashCharacter

	const float RandomRate = FMath::FRandRange(0.f, 1.f);
	if(RandomRate < 0.5f)
	{
		SpawnSoul(); //50%生成Soul,50%生成Recover
	}
	else
	{
		SpawnRecover();
	}
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if(World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if(SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSoulCount());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::SpawnRecover()
{
	//不能在这里写随机，这会导致所有敌人的Die中的SpawnRecover概率一致
	// const float RandomRate = FMath::FRandRange(0.f, 1.f);
	// if(RandomRate < 0.5f)
	// {
	// 	return; //50%的概率不生成。
	// }
	
	UWorld* World = GetWorld();
	if(World && RecoverClass && Attributes)
	{
		ARecover* SpawnedRecover = World->SpawnActor<ARecover>(RecoverClass, GetActorLocation() + FVector(0.f, 0.f, 100.f), GetActorRotation());
		if(SpawnedRecover)
		{
			SpawnedRecover->SetRecoverAmount(Attributes->GetRecoverCount());
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if(!CombatTarget)
	{
		return;
	}
	
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	//不能加上Super::CanAttack();源代码返回false
	const bool bCanAttack = IsInsideAttackRadius() &&
		bIsSeeingPlayer &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();//确保
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if(Attributes && HealthBarWidgetComponent)
	{
		//Attributes->ReceiveDamage(DamageAmount);  已经在Super::HandleDamage(DamageAmount);中写了
		HealthBarWidgetComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

// int32 AEnemy::PlayDeathMontage()
// {
// 	const int32 Selection = Super::PlayDeathMontage();
// 	//重点
// 	TEnumAsByte<EDeathPose> Pose(Selection);
// 	if(Pose < EDeathPose::EDP_MAX)//EDP_MAX的作用就在这里，确定其在最后，所有可以根据EDP_MAX推断出总数
// 	{
// 		DeathPose = Pose;
// 	}
// 	return Selection;
// }

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	HideHealthBar();
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::CheckCombatTarget()
{
	//combat包括chasing,Attacking和Engaged
	if(IsOutsideCombatRadius())
	{
		//Outside combat radius, lose interest
		ClearAttackTimer();//千万别忘了
		LoseInterest();
		if(!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if(IsOutsideAttackRadius() && !IsChasing())//直接加 && !IsAttacking()有bug，需要修复
	{
		// Outside attack range, chase character
		ClearAttackTimer();//比如在攻击时目标突然离开攻击范围，那么就没必要再攻击了
		if(!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if(CanAttack())
	{
		// Inside attack range, attack character
		StartAttackTimer();//只要再次start，就没必要clear定时器了
	}
}

void AEnemy::CheckPatrolTarget()
{
	if(InTargetRange(PatrolTarget,PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(
			PatrolTimer,
			this,
			&AEnemy::PatrolTimerFinished,
			PatrolWaitTime
		);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if(HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if(HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	if(Attributes)
	{
		Attributes->RecoverHealthToMax();
		HealthBarWidgetComponent->SetHealthPercent(1.f);
	}
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);//重新设置Moveto的目标
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	ShowHealthBar();
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget,CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::FRandRange(AttackMin,AttackMax);
	GetWorldTimerManager().SetTimer(
		AttackTimer,
		this,
		&AEnemy::Attack,
		AttackTime
	);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if(Target == nullptr) return false;
	const double& DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	//DRAW_SPHERE_SingleFrame(GetActorLocation());
	//DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if(EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);//会站在更远的地方就进行攻击
	//FNavPathSharedPtr NavPath;//用来绘制导航的点的
	EnemyController->MoveTo(MoveRequest);//,&NavPath
	//TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();//加上&避免复制
	// for(auto& PathPoint:PathPoints)
	// {
	// 	const FVector& Location = PathPoint.Location;
	// 	DrawDebugSphere(GetWorld(),Location,12.f,12,FColor::Green,false,10.f);
	// }
}

AActor* AEnemy::ChoosePatrolTarget()
{
	//避免每一帧都setGoalActor、AcceptanceRadius等等
	TArray<AActor*> ValidTargets;
	for(AActor* Target: PatrolTargets)
	{
		if(Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);//关键
		}
	}
			
	const int32 NumPatrolTargets = ValidTargets.Num();
	if(NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0,NumPatrolTargets-1);
		return ValidTargets[TargetSelection];
	}
	
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if(World && WeaponClass)
	{
		//适用于装备上武器蓝图（单纯静态网格体不行），而且BeginPlay以后才会装备，所以会听到敌人装备武器的声音，在self处的Weapon Class可以看到
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(),FName("WeaponSocket"),this,this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngagedTarget"));//比Cast<ASlashCharacter>(SeenPawn)更好,tag在SlashCharacter.cpp中的BeginPlay()先添加好
	
	if(!bShouldChaseTarget) return; //防止每次都遍历所有内容
	CombatTarget = SeenPawn;
	bIsSeeingPlayer = true;
	ClearPatrolTimer();//注意clear
	ChaseTarget();
}
