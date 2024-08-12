// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;
class UEscOverlay;

UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<USlashOverlay> SlashOverlayClass;
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UEscOverlay> EscOverlayClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	USlashOverlay* SlashOverlay;
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	UEscOverlay* EscOverlay;

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
	FORCEINLINE UEscOverlay* GetEscOverlay() const { return EscOverlay; }

	void AddPauseMenuToViewport(APlayerController* Controller);
	void RemovePauseMenuFromViewport();
};
