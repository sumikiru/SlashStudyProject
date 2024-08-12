// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	UPROPERTY(BlueprintReadOnly)//不需要编辑器可见
	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;
	//不能使用UFUNCTION()来进行override，否则会导致编译错误
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	void ExecuteGetHit(FHitResult BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

	//并不是重写override，所以需要UFUNCTION()
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)	//这样就不需要在C++中定义函数了
	void CreateFields(const FVector& FieldLocation);
private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FVector BoxTraceExtent = FVector(5.0f, 5.0f, 5.0f);//等价于FVector(5.f);
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bShowBoxDebug = false;
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	USoundBase* EquipSound;
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage = 20.f;

public:
	//Getters(to get WeaponBox)
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
