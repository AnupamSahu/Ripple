// Copyright Anupam Sahu. All Rights Reserved.

#include "ModuleMaker/FRpModuleMaker.h"
#include "ModuleMaker/SRpModuleMakerWidget.h"

#include "Misc/FileHelper.h"
#include "GameProjectUtils.h"

#include "Interfaces/IPluginManager.h"
#include "Interfaces/IMainFrameModule.h"

#define MAX_MODULE_NAME_LENGTH 32

void FRpModuleMaker::LaunchModuleCreationDialog()
{
	const IMainFrameModule& MainFrameModule = FModuleManager::GetModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const FVector2D ModuleMakerWindowSize(960, 540);
	
	const TSharedRef<SWindow> ModuleCreationWindow =
		SNew(SWindow)
		.Title(INVTEXT("Create Module"))
		.ClientSize(ModuleMakerWindowSize)
		.SizingRule(ESizingRule::FixedSize)
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.SupportsMinimize(false)
		.SupportsMaximize(false);

	ModuleCreationWindow->SetContent(SNew(SRpModuleMakerWidget));
	
	if (MainFrameModule.GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(ModuleCreationWindow, MainFrameModule.GetParentWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(ModuleCreationWindow);
	}
}

bool FRpModuleMaker::IsModuleNameValid(const FString& NameString, FString& RejectReason) 
{
	RejectReason.Empty();
	
	if(NameString.IsEmpty())
	{
		RejectReason.Append("You must specify a module name.");
		return false;
	}

	if(NameString.Contains(" "))
	{
		RejectReason.Append("Your module name may not contain a space.");
		return false;
	}

	if(!FChar::IsAlpha(NameString[0]))
	{
		RejectReason.Append("Your module name must begin with an alphabetic character.");
		return false;
	}

	if(NameString.Len() > MAX_MODULE_NAME_LENGTH)
	{
		TArray <FStringFormatArg> Args;
		Args.Add(MAX_MODULE_NAME_LENGTH);
		
		RejectReason.Append(FString::Format(TEXT("The module name must be no longer than {0} characters."), Args));

		return false;
	}

	FString IllegalNameCharacters;
	if ( !GameProjectUtils::NameContainsOnlyLegalCharacters(NameString, IllegalNameCharacters) )
	{
		TArray<FStringFormatArg> Args;
		Args.Add(IllegalNameCharacters);

		RejectReason.Append(FString::Format(TEXT("The class name may not contain the following characters: '{0}'"), Args));

		return false;
	}
	return true;
}

bool FRpModuleMaker::IsModuleSourcePathValid(const FString& PathString, const FString& ModuleName, FString& RejectReason)
{
	RejectReason.Empty();

	// Module Name check
	if(ModuleName.IsEmpty())
	{
		RejectReason.Append("Unable to create a module without a name.");
		return false;
	}

	// Directory check
	if(PathString.IsEmpty() || !FPaths::DirectoryExists(PathString) || !FPaths::ValidatePath(PathString))
	{
		RejectReason.Append("Module directory path is not valid.");
		return false;
	}

	// Existing source files check
	{
		const FString& AbsolutePath = FPaths::ConvertRelativePathToFull(PathString) / "";
		const FString& ModulePath = AbsolutePath + ModuleName / "";

		const FString& HeaderPath = ModulePath / "public" / ModuleName + ".h";
		const FString& SourcePath = ModulePath / "private" / ModuleName + ".cpp";
		const FString& ConfigPath = ModulePath / ModuleName + ".build.cs";
	
		const bool bSourceFilesAlreadyExist = FPaths::FileExists(HeaderPath) || FPaths::FileExists(SourcePath) || FPaths::FileExists(ConfigPath);

		if(bSourceFilesAlreadyExist)
		{
			RejectReason.Append("Module source files already exist in the chosen folder.");
			return false;
		}
	}
	

	return true;
}

bool FRpModuleMaker::CreateNewModule(const FString& ModuleName, const FString& ModulePath, FString& OutFailReason)
{
	OutFailReason.Empty();
	
	if(!FPaths::DirectoryExists(ModulePath))
	{
		OutFailReason.Append("Module path does not exist.");
		return false;
	}

	const FString& ModuleDirectory = ModulePath / ModuleName;
	
	const FString& HeaderPath = ModuleDirectory / "Public" / ModuleName + "Module.h";
	const bool HeaderSuccess = CreateModuleSourceFile(ModuleName, "Module.h.template", HeaderPath, OutFailReason);
	
	const FString& SourcePath = ModuleDirectory / "Private" / ModuleName + "Module.cpp";
	const bool SourceSuccess = CreateModuleSourceFile(ModuleName, "Module.cpp.template", SourcePath, OutFailReason);
	
	const FString& ConfigPath = ModuleDirectory / ModuleName + ".build.cs";
	const bool ConfigSuccess = CreateModuleSourceFile(ModuleName, "Module.build.cs.template", ConfigPath, OutFailReason);
	
	if(!HeaderSuccess || !SourceSuccess || !ConfigSuccess)
	{
		OutFailReason.Append("Failed to add some source files.");
		return false;
	}

	return true;
}

bool FRpModuleMaker::CreateModuleSourceFile(const FString& TemplateValue, const FString& TemplateFile, const FString& TargetFile, FString& OutFailReason)
{
	// TODO : I may need to add a dedicated class to provide me with plugin directories specific to a Ripple.
	const FString& PluginContentDir = IPluginManager::Get().FindPlugin(TEXT("Ripple"))->GetContentDir();
	const FString& TemplateDir = PluginContentDir / "Editor" / "Templates" / ""; 
	
	FString Content;
	if(!FFileHelper::LoadFileToString(Content, *(TemplateDir / TemplateFile)))
	{
		OutFailReason.Append("Failed to read template : " + TemplateFile + ". ");
		return false;
	}
	
	Content = Content.Replace(TEXT("[MODULENAME]"), *TemplateValue);
	if(!FFileHelper::SaveStringToFile(Content, *TargetFile))
	{
		OutFailReason.Append("Failed to add source : " + TargetFile + ". ");
		return false;
	}
	
	return true;
}
