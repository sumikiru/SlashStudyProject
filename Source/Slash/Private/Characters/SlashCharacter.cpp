// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "GroomComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Item.h"
#include "Items/Recover.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//使用控制器所需的旋转=false;
	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll=false;

	//将旋转朝向运动=true;(BP中的CharacterMovementComponent中)
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,360.0f,0.0f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);//对于触发蓝图逻辑很重要

	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength=300.0f;
	//使用Pawn控制旋转=true;(BP的USpringArmComponent中)
	CameraBoom->bUsePawnControlRotation=true;

	ViewCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair=CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());//Mesh是私有的，需要使用get方法
	Hair->AttachmentName=FString("head");
	
	Eyebrows=CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName=FString("head");
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		if(Attributes->GetGoldCount() > UpgradeGoldCount)
		{
			Upgrade();
		}
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//记得勾选使用pawn控制旋转(BP中的CameraComponent中)
	//使用控制器所需的旋转=false;将旋转朝向运动=true;(BP中的CharMoveComp中)
	PlayerInputComponent->BindAxis(FName("MoveForward"),this,&ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"),this,&ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"),this,&ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"),this,&ASlashCharacter::LookUp);

	//只需要按一次的使用BindAction 
	PlayerInputComponent->BindAction(FName("Jump"),IE_Pressed,this,&ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"),IE_Pressed,this,&ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"),IE_Pressed,this,&ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter::Dodge);
	//直接在最后面加上.bExecuteWhenPaused=true就可以实现暂停时按键仍然可以触发！！！
	PlayerInputComponent->BindAction(FName("Pause"), IE_Pressed, this, &ASlashCharacter::PauseGame).bExecuteWhenPaused = true;
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();
	if(Attributes && Attributes->GetHealthPercent() > 0.f)//避免死亡后ActionState还是EAS_HitReaction
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSoulCount(Attributes->GetSoulCount());
	}
}

void ASlashCharacter::AddGold(ATreasure* Gold)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->AddGold(Gold->GetGold());
		SlashOverlay->SetGoldCount(Attributes->GetGoldCount());
	}
}

void ASlashCharacter::AddHealth(ARecover* Recover)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->AddHealth(Recover->GetRecoverAmount());
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::DisableWeaponCollision()
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngagedTarget"));//添加标签

	InitializeSlashOverlay();
}

void ASlashCharacter::MoveForward(float Value)
{
	if (!IsUnoccupied())
	{
		return;
	}
	if (Controller && Value != 0.0f)
	{
		//让角色向着屏幕正前方移动,而非角色的正前方
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (!IsUnoccupied())
	{
		return;
	}
	if(Controller&&Value!=0.0f)
	{
		const FRotator ControlRotation=GetControlRotation();
		const FRotator YawRotation(0.0f,ControlRotation.Yaw,0.0f);

		const FVector Direction=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,Value);
	}
}

void ASlashCharacter::Jump()
{
	if (!IsUnoccupied())
	{
		return;
	}
	Super::Jump();
}

void ASlashCharacter::Turn(float Value)
{
	if(!IsAlive())
	{
		return;
	}
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	if(!IsAlive())
	{
		return;
	}
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		//限制只能装备一种武器
		if(CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon) return;
		//equip
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		//disarm
		if(CanDisarm())
		{
			UnEquipWeapon();
		}
		//arm
		else if(CanArm())
		{
			EquipWeapon();
		}
	}
}

void ASlashCharacter::Dodge()
{
	if(IsOccupied() || !HasEnoughStamina())
	{
		return;
	}
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	Attributes->UseStamina(Attributes->GetDodgeStaminaCost());
	Attributes->SetDodgeStaminaRegenRate();
}

void ASlashCharacter::PauseGame()
{
	//SetTickableWhenPaused(true);
	UWorld* World = GetWorld();
	if (!World) return;

	//UE_LOG(LogTemp,Warning, TEXT("PauseGame"));
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			if (bIsWorldPausing)
			{
				SlashHUD->RemovePauseMenuFromViewport();
				bIsWorldPausing = false;
			}
			else
			{
				SlashHUD->AddPauseMenuToViewport(PlayerController);
				bIsWorldPausing = true;
			}
		}
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName,EquipMontage);
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr; //注意！表示已经完成装备，不需要再与OverlappingItem进行交互
	EquippedWeapon = Weapon;
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
		if(Attributes)
		{
			Attributes->UseStamina(Attributes->GetDodgeStaminaCost());
			Attributes->SetDodgeStaminaRegenRate();
		}
	}
}

//就算这里写了该函数，仍然需要在蓝图中调用AttackEnd(),不会默认使用到的！
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

	if(Attributes)
	{
		Attributes->SetUnoccupiedStaminaRegenRate();
	}
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
	if(Attributes)
	{
		Attributes->SetUnoccupiedStaminaRegenRate();
	}
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::HasEnoughStamina() const
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeStaminaCost();
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();//注意也要改

	//死亡后不再接受任何动作
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
	//StopAttackMontage()和DisableWeaponCollision()都已经在GetHit_Implementation中实现
	//StopAttackMontage();
	//DisableWeaponCollision();

	// LatentInfo, 用Timer也可以
	// Param1 : Linkage 默认给0
	// Param2 : UUID 给个独一无二的编码即可  FMath::Rand()  或者给时间戳转FString + 类名 也可保证唯一性
	// Param3 : ExecutionFunction 要执行的方法名  用TEXT宏转一下
	// Param4 : CallbackTarget  回调传入的UObject目标 传this
	const FLatentActionInfo	LatentActionInfo(0,
		FMath::Rand(),
		TEXT("PauseGame"),
		this
	);
	UKismetSystemLibrary::Delay(
		this,
		3.f,
		LatentActionInfo
	);
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
			EquippedWeapon;
}

void ASlashCharacter::UnEquipWeapon()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState=ECharacterState::ECS_Unequipped;
	ActionState=EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::EquipWeapon()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState=EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Disarm()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(),FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(),FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsUnoccupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsOccupied() const
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Upgrade()
{
	Attributes->Upgrade();
	SetHUDHealth();//必须这样才能更改UI
	
	const int32 NewGoldCount = FMath::Clamp(Attributes->GetGoldCount() - UpgradeGoldCount, 0, Attributes->GetGoldCount());
	UpgradeGoldCount += 5;
	Attributes->SetGoldCount(NewGoldCount);//别忘了，这个才是真正属性
	SetHUDGoldCount();//只是更新显示的数字，并没有更改属性值
	//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("升级，当前生命值为：%f\nUpgradeGoldCount: %d"),Attributes->GetHealth(),UpgradeGoldCount));
	//UE_LOG(LogTemp,Warning, TEXT("升级，当前生命值为：%f\nUpgradeGoldCount: %d"),Attributes->GetHealth(),UpgradeGoldCount);
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if(SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if(SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGoldCount(0);
				SlashOverlay->SetSoulCount(0);
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth() const
{
	if(SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::SetHUDGoldCount() const
{
	if(SlashOverlay && Attributes)
	{
		SlashOverlay->SetGoldCount(Attributes->GetGoldCount());
	}
}

void ASlashCharacter::SetHUDSoulCount() const
{
	if(SlashOverlay && Attributes)
	{
		SlashOverlay->SetSoulCount(Attributes->GetSoulCount());
	}
}

void ASlashCharacter::SetHUDStamina() const
{
	if(SlashOverlay && Attributes)
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}
