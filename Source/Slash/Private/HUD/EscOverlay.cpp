// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EscOverlay.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UEscOverlay::ResumeSlashGame()
{
	UWorld* World = GetWorld();
	if(!World)
	{
		return;
	}

	APlayerController* Controller = World->GetFirstPlayerController();
	Controller->bShowMouseCursor = false;
	Controller->bBlockInput = false;
	Controller->bEnableClickEvents = true;
	//等价于蓝图的SetInputModeGameOnly，支持鼠标和键盘输入
	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);
	
	UGameplayStatics::SetGamePaused(World, false);
	RemoveFromParent();//将改控件从父控件中移除
}

void UEscOverlay::RestartGame()
{
	UWorld* World = GetWorld();
	if(!World)
	{
		return;
	}
	RemoveFromParent();//将该控件从父控件中移除
	//重新开始游戏
	FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
	UGameplayStatics::OpenLevel(World, FName(*LevelName));

	APlayerController* Controller = World->GetFirstPlayerController();
	Controller->bShowMouseCursor = false;
	Controller->bBlockInput = false;
	Controller->bEnableClickEvents = true;
	//等价于蓝图的SetInputModeGameOnly，支持鼠标和键盘输入
	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);
}

void UEscOverlay::QuitSlashGame()
{
	UWorld* World = GetWorld();
	if(!World)
	{
		return;
	}
	APlayerController* PlayerController = World->GetFirstPlayerController();
	
	bool bIsQuitGame = World &&
		PlayerController &&
		ExitButton &&
		ExitButton->OnClicked.IsBound();
	if(bIsQuitGame)
	{
		UKismetSystemLibrary::QuitGame(
			World,
			PlayerController,
			EQuitPreference::Quit,
			false
		);
	}
}

void UEscOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	//必须在NativeConstruct()中添加控件的点击事件，否则无效
	//需为AddDynamic，且第二个不能为常函数
	ResumeButton->OnClicked.AddDynamic(this, &UEscOverlay::ResumeSlashGame);
	RestartButton->OnClicked.AddDynamic(this, &UEscOverlay::RestartGame);
	ExitButton->OnClicked.AddDynamic(this, &UEscOverlay::QuitSlashGame);//QuitSlashGame不能设置为常函数，会报错
}

/* 不可用 
void UEscOverlay::ClickedResumeButton()
{
	FScriptDelegate ResumeDelegate;
	ResumeDelegate.BindUFunction(this, "ResumeSlashGame");
	ResumeButton->OnClicked.Add(ResumeDelegate);
}

void UEscOverlay::ClickedOptionsButton()
{
	FScriptDelegate OptionsDelegate;
	OptionsDelegate.BindUFunction(this, "RestartGame");
	RestartButton->OnClicked.Add(OptionsDelegate);
}

void UEscOverlay::ClickedExitButton()
{
	FScriptDelegate ExitDelegate;
	ExitDelegate.BindUFunction(this, "QuitSlashGame");
	ExitButton->OnClicked.Add(ExitDelegate);
	ExitButton->OnClicked.AddDynamic(this, &UEscOverlay::QuitSlashGame);//QuitSlashGame不能设置为常函数，会报错
}
*/
