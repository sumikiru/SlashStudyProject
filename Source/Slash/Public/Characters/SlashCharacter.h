 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "Characters/BaseCharacter.h"
#include "SlashCharacter.generated.h"

 class UEscOverlay;
class ATreasure;
class USlashOverlay;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	/** <ACharacter> */
	virtual void Tick(float DeltaTime) override;
	/** </ACharacter> */
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Gold) override;
	virtual void AddHealth(ARecover* Recover) override;
	
	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

protected:
	virtual void BeginPlay() override;

	/** <Callbacks for Input> */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void Dodge();

	UFUNCTION()//否则Die_Implementation()中的FLatentActionInfo的InFunctionName无法找到对应函数
	void PauseGame();
	/** </Callbacks for Input> */
	
	/** <Play Specific Montage> */
	void PlayEquipMontage(const FName& SectionName);
	/** </Play Specific Montage> */

	/** <Combat Related> */
	void EquipWeapon(AWeapon* Weapon);
	virtual void Attack() override;
	virtual void AttackEnd() override;//因为父类已经有UFUNCTION(BlueprintCallable)，仅限于蓝图调用时这么做
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool HasEnoughStamina() const;
	virtual void Die_Implementation() override;
	bool CanDisarm();
	bool CanArm();
	void UnEquipWeapon();
	void EquipWeapon();

	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
	/** </Combat Related> */

private:
	bool IsUnoccupied() const;
	bool IsOccupied() const;
	void Upgrade();
	
	/** <Character Components> */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(VisibleAnywhere,Category="Hair")
	UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere,Category="Hair")
	UGroomComponent* Eyebrows;
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	/** </Character Components> */
	
	/** <Animation Montage> */
	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* EquipMontage;
	/** </Animation Montage> */

	/** <States> */
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	int32 UpgradeGoldCount = 10;
	/** </States> */

	/** <Slash HUD> */
	UPROPERTY()
	USlashOverlay* SlashOverlay;
	UPROPERTY()
	UEscOverlay* EscOverlay;

	UPROPERTY(EditAnywhere)
	bool bIsWorldPausing = false;

	void InitializeSlashOverlay();
	void SetHUDHealth() const;
	void SetHUDGoldCount() const;
	void SetHUDSoulCount() const;
	void SetHUDStamina() const;
	/** </Slash HUD> */
	
public:
	/** <Getter and Setter> */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	/** </Getter and Setter> */
};
