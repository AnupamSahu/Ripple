// Copyright Anupam Sahu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FRippleEditorModule : public IModuleInterface
{
public:

	// Initialize the Module, and extend Menus.
	virtual void StartupModule() override;

	// Destroy all extensions.
	virtual void ShutdownModule() override;
	
	// Add a Main Menu Entry to the Menu Bar.
	static void AddMenuExtension(FMenuBarBuilder& MenuBuilder);

	// Provides support to extend the Main Menu.
	TSharedPtr<FExtender> MainMenuExtender;

	// The extension object that results form the Main Menu Extension.
	TSharedPtr<const FExtensionBase> Extension;
};
