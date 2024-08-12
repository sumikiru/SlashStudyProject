// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscOverlay.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class SLASH_API UEscOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void ResumeSlashGame();
	UFUNCTION()
	void RestartGame();
	UFUNCTION()
	void QuitSlashGame();

	// void ClickedResumeButton();
	// void ClickedOptionsButton();
	// void ClickedExitButton();
private:
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
