// Fill out your copyright notice in the Description page of Project Settings.

#include "FRippleActions.h"

#include "ModuleMaker/FModuleMaker.h"
#include "LoggingMacros.h"

TSharedRef<FUICommandList> FRippleActions::ActionList(new FUICommandList());

FRippleActions::FRippleActions()
	: TCommands<FRippleActions>(FName(TEXT("RPL_MainMenu")), FText::FromString("Ripple Main Menu Commands"), NAME_None, FEditorStyle::GetStyleSetName())
{
}

void FRippleActions::RegisterCommands()
{
	#define LOCTEXT_NAMESPACE ""

	UI_COMMAND(CreateModule, "Create a new module", "Use Ripple's automatic module creation feature to quickly create new modules for your project.", EUserInterfaceActionType::Button, FInputChord());
	ActionList->MapAction(CreateModule, FExecuteAction::CreateStatic(&FRippleActionCallbacks::CreateModule), FCanExecuteAction());

	#undef LOCTEXT_NAMESPACE
}

void FRippleActions::FillMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("Modules", FText::FromString("Modules"));
	{
		MenuBuilder.AddMenuEntry
		(
			FRippleActions::Get().CreateModule, NAME_None,
			FText::FromString("Create Module"),
			FText::FromString("Create a new module"),
			FSlateIcon()
		);
	}
}

void FRippleActionCallbacks::CreateModule()
{
	CLOG(Log, "Create module requested.")
	FModuleMaker::LaunchModuleCreationDialog();
}
