// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include <HUD/SlashOverlay.h>
#include "HUD/EscOverlay.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if(Controller && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);//创建控件
			SlashOverlay->AddToViewport();
		}
	}
	
}

void ASlashHUD::AddPauseMenuToViewport(APlayerController* Controller)
{
	UWorld* World = GetWorld();
	if(World)
	{
		if(Controller && EscOverlayClass)
		{
			EscOverlay = CreateWidget<UEscOverlay>(Controller, EscOverlayClass);//创建控件
			EscOverlay->AddToViewport();

			Controller->bShowMouseCursor = true;
			Controller->bBlockInput = false;
			Controller->bEnableClickEvents = true;
			//等价于蓝图的SetInputModeGameAndUI，支持鼠标和键盘输入
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(EscOverlay->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);//是否锁定鼠标在视窗内
			InputMode.SetHideCursorDuringCapture(false);
			Controller->SetInputMode(InputMode);
		}
		UGameplayStatics::SetGamePaused(World, true);
	}
}

void ASlashHUD::RemovePauseMenuFromViewport()
{
	UWorld* World = GetWorld();
	if (World && EscOverlay)
	{
		EscOverlay->ResumeSlashGame();
	}
}
