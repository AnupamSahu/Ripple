// Fill out your copyright notice in the Description page of Project Settings.

#include "ModuleMaker/SModuleMakerWidget.h"

#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SGridPanel.h"

#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"

#include "Misc/FileHelper.h"
#include "ModuleMaker/FModuleMaker.h"

#define MAX_MODULE_NAME_LENGTH 32

BEGIN_FUNCTION_BUILD_OPTIMIZATION
void SModuleMakerWidget::Construct(const FArguments& InArgs)
{
	constexpr float TextBoxHeightOverride = 24.0f;
	
	ChildSlot
	[
		SNew(SBorder)
		.Padding(18)
		.BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
		[
			SNew(SVerticalBox)


#pragma region Title
			// Title 
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "NewClassDialog.PageTitle")
				.Text(INVTEXT("Name Your New Module"))
			]

			// Title spacer
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 3, 0, 8)
			[
				SNew(SSeparator)
			]
#pragma endregion 

#pragma region Description
			// Description
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 5)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 5)
				[
					SNew(STextBlock)
					.Text(INVTEXT("Enter a name for your new module. Module names may only contain alphanumeric characters, and may not contain a space."))
				]

				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 5)
				[
					SNew(STextBlock)
					.Text(INVTEXT("When you click the \"Create\" button below, a header (.h) file, a source (.cpp) and a build configuration (.cs) file will be made using this name."))
				]
			]
#pragma endregion

#pragma region Error Text
			// Name Error label
			+SVerticalBox::Slot()
			.Padding(0, 0, 0, 5)
			.AutoHeight()
			[
				// Constant height, whether the label is visible or not
				SNew(SBox).HeightOverride(20)
				[
					SNew(SBorder)
					.Visibility( this, &SModuleMakerWidget::GetErrorLabelVisibility)
					.BorderImage(FEditorStyle::GetBrush("NewClassDialog.ErrorLabelBorder"))
					.Content()
					[
						SNew(STextBlock)
						.Text(this, &SModuleMakerWidget::GetErrorLabelText)
						.TextStyle(FEditorStyle::Get(), "NewClassDialog.ErrorLabelFont")
					]
				]
			]
#pragma endregion

#pragma region Details View
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
				.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f ))
				.Padding(FMargin(6.0f, 4.0f, 7.0f, 4.0f))
				[
					SNew(SGridPanel)
					.FillColumn(0, 0.15f)
					.FillColumn(1, 0.85f)

					// Name Label
					+ SGridPanel::Slot(0, 0)
					.Padding(0, 3, 0, 0)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(),"NewClassDialog.SelectedParentClassLabel")
						.Text(INVTEXT("Name"))
					]

					// Name Input Text Box
					+SGridPanel::Slot(1, 0)
					.Padding(0, 3, 0, 0)
					[
						SNew(SBox)
						.HeightOverride(TextBoxHeightOverride)
						[
							SAssignNew(NameEditBox, SEditableTextBox)
							.Text(this, &SModuleMakerWidget::GetModuleName)
							.HintText(INVTEXT("Name of your module"))
							.OnTextChanged(this, &SModuleMakerWidget::OnModuleNameChanged)
							.OnTextCommitted(this, &SModuleMakerWidget::OnModuleNameCommitted)
						]
					]
					
					// Path Label
					+ SGridPanel::Slot(0, 1)
					.VAlign(VAlign_Center)
					.Padding(0, 3, 0, 0)
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel")
						.Text(INVTEXT("Path"))
					]
					
					// Path input text box
					+ SGridPanel::Slot(1, 1)
					.VAlign(VAlign_Center)
					.Padding(0, 3, 0, 0)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						[
							SNew(SBox)
							.HeightOverride(TextBoxHeightOverride)
							[
								SNew(SEditableTextBox)
								.Text(this, &SModuleMakerWidget::GetModulePath)
								.HintText(INVTEXT("Location of your module"))
								.IsReadOnly(true)
							]
						]
						
						// Choose Folder Button
						+SHorizontalBox::Slot()
						.Padding(5, 0, 0, 0)
						.AutoWidth()
						[
							SNew(SButton)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.Visibility(this, &SModuleMakerWidget::GetChooseFolderVisibility)
							.OnClicked(this, &SModuleMakerWidget::HandleChooseFolderButtonClicked)
							.Text(INVTEXT("Choose Folder"))
						]
					]

					// Header File Label
					+SGridPanel::Slot(0, 2)
					.VAlign(VAlign_Center)
					.Padding(0, 3, 0, 0)
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel")
						.Text(INVTEXT("Header File"))
					]

					// Header File Path
					+SGridPanel::Slot(1, 2)
					.Padding(5, 3, 0, 0)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HeightOverride(TextBoxHeightOverride)
						[
							SNew(STextBlock)
							.Text(this, &SModuleMakerWidget::GetHeaderFilePath)
						]
					]

					// Source File Label
					+SGridPanel::Slot(0, 3)
					.VAlign(VAlign_Center)
					.Padding(0, 3, 0, 0)
					[
						SNew(STextBlock)
						.TextStyle( FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel" )
						.Text(INVTEXT("Source File"))
					]

					// Source File Path
					+SGridPanel::Slot(1, 3)
					.Padding(5, 3, 0, 0)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HeightOverride(TextBoxHeightOverride)
						[
							SNew(STextBlock)
							.Text(this, &SModuleMakerWidget::GetSourceFilePath)
						]
					]

					// Build Configuration File Label
					+SGridPanel::Slot(0, 4)
					.VAlign(VAlign_Center)
					.Padding(0, 3, 0, 0)
					[
						SNew(STextBlock)
						.TextStyle( FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel" )
						.Text(INVTEXT("Build Config File"))
					]

					// Build Configuration File Path
					+SGridPanel::Slot(1, 4)
					.Padding(5, 3, 0, 0)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HeightOverride(TextBoxHeightOverride)
						[
							SNew(STextBlock)
							.Text(this, &SModuleMakerWidget::GetConfigFilePath)
						]
					]
				]
			]
#pragma endregion

#pragma region Menu Buttons
			+SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)

				// Create Module Button
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.IsEnabled(this, &SModuleMakerWidget::GetCreateButtonAbility)
					.ContentPadding(FMargin(25.0f, 5.0f))
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
					.TextStyle(FEditorStyle::Get(), "LargeText")
					.ForegroundColor(FEditorStyle::Get().GetSlateColor("WhiteBrush"))
					.Text(INVTEXT("Create Module"))
					.OnClicked(this, &SModuleMakerWidget::HandleCreateButtonClicked)
				]

				// Cancel Button
				+SHorizontalBox::Slot()
				.Padding(10, 0, 0, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ContentPadding(FMargin(25.0f, 5.0f))
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
					.TextStyle(FEditorStyle::Get(), "LargeText")
					.ForegroundColor(FEditorStyle::Get().GetSlateColor("WhiteBrush"))
					.Text(INVTEXT("Cancel"))
					.OnClicked(this, &SModuleMakerWidget::HandleCancelButtonClicked)
				]
			]
#pragma endregion
		]
	];
}
END_FUNCTION_BUILD_OPTIMIZATION

EVisibility SModuleMakerWidget::GetErrorLabelVisibility() const
{
	return ErrorString.IsEmpty() ? EVisibility::Hidden : EVisibility::Visible;
}

FText SModuleMakerWidget::GetErrorLabelText() const
{
	return FText::AsCultureInvariant(ErrorString);
}

EVisibility SModuleMakerWidget::GetChooseFolderVisibility() const
{
	return bLastNameValidityCheckPassed ? EVisibility::Visible : EVisibility::Collapsed;
}

bool SModuleMakerWidget::GetCreateButtonAbility() const
{
	return bLastNameValidityCheckPassed && bLastPathValidityCheckPassed;
}

FText SModuleMakerWidget::GetModuleName() const
{
	return FText::AsCultureInvariant(NewModuleName);
}

FText SModuleMakerWidget::GetModulePath() const
{
	return FText::AsCultureInvariant(NewModulePath);
}

FText SModuleMakerWidget::GetHeaderFilePath() const
{
	return FText::AsCultureInvariant(HeaderFile);
}

FText SModuleMakerWidget::GetSourceFilePath() const
{
	return FText::AsCultureInvariant(SourceFile);
}

FText SModuleMakerWidget::GetConfigFilePath() const
{
	return FText::AsCultureInvariant(ConfigFile);
}

void SModuleMakerWidget::UpdateSourceFilePaths(bool ClearPaths)
{
	if(ClearPaths)
	{
		HeaderFile = SourceFile = ConfigFile = "";
		return;
	}

	const FString& ModuleDirectory = NewModulePath / NewModuleName;
	
	HeaderFile = ModuleDirectory / "public" / NewModuleName + ".h";
	SourceFile = ModuleDirectory / "private" / NewModuleName + ".cpp";
	ConfigFile = ModuleDirectory / NewModuleName + ".build.cs";
}

void SModuleMakerWidget::UpdateValidityChecks()
{
	bLastNameValidityCheckPassed = FModuleMaker::IsModuleNameValid(NewModuleName, ErrorString);
	bLastPathValidityCheckPassed = FModuleMaker::IsModuleSourcePathValid(NewModulePath, NewModuleName, ErrorString);
}

void SModuleMakerWidget::OnModuleNameChanged(const FText& ModuleNameInput)
{
	const FString& ModuleNameInputString = ModuleNameInput.ToString();
	bLastNameValidityCheckPassed = FModuleMaker::IsModuleNameValid(ModuleNameInputString, ErrorString);
	if(bLastNameValidityCheckPassed)
	{
		NewModuleName = ModuleNameInputString;
		if(bLastPathValidityCheckPassed)
		{
			UpdateSourceFilePaths();
		}
	}
	else
	{
		UpdateSourceFilePaths(true);
	}
}

void SModuleMakerWidget::OnModuleNameCommitted(const FText& ModuleNameInput, ETextCommit::Type CommitType)
{
	if(CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
	{
		if(NameEditBox.IsValid())
		{
			if(bLastNameValidityCheckPassed)
			{
				NameEditBox->SetText(ModuleNameInput);
			}
			else
			{
				NameEditBox->SetText(INVTEXT(""));
				ErrorString = "You must specify a module name.";
			}
		}
	}
}

void SModuleMakerWidget::HandleFolderChosen(FString& FolderName)
{
	if(!FolderName.EndsWith(TEXT("/"))) { FolderName += TEXT("/"); }

	ErrorString.Empty();
	bLastPathValidityCheckPassed = FModuleMaker::IsModuleSourcePathValid(FolderName, NewModuleName, ErrorString);
	if(bLastPathValidityCheckPassed)
	{
		NewModulePath = FolderName;
		UpdateSourceFilePaths();
	}
	else
	{
		UpdateSourceFilePaths(true);
	}
}

FReply SModuleMakerWidget::HandleChooseFolderButtonClicked()
{
	if(bLastNameValidityCheckPassed)
	{
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if(DesktopPlatform != nullptr)
		{
			const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
			const void* ParentWindowHandle = (ParentWindow.IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

			FString FolderName;
			const FString& Title = "Choose a source location";
			const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, Title, FPaths::ProjectDir(), FolderName);

			if(bFolderSelected)
			{
				HandleFolderChosen(FolderName);
			}
		}
	}
	
	return FReply::Handled();
}

FReply SModuleMakerWidget::HandleCreateButtonClicked()
{
	UpdateValidityChecks();
	if(bLastNameValidityCheckPassed && bLastPathValidityCheckPassed)
	{
		FString OutFailReason;
		FFormatOrderedArguments Args;
		
		if(!FModuleMaker::CreateNewModule(NewModuleName, NewModulePath, OutFailReason))
		{
			if(!OutFailReason.IsEmpty())
			{
				Args.Add(FText::AsCultureInvariant(OutFailReason));
			
				const FText& FailureMessage = FText::Format(INVTEXT("Operation unsuccessful. {0}"), Args);
				FMessageDialog::Open(EAppMsgType::Ok, FailureMessage);	
			}
			return FReply::Handled();
		}

		Args.Add(FText::AsCultureInvariant(NewModuleName));
		Args.Add(FText::AsCultureInvariant(NewModulePath));
			
		const FText& SuccessMessage = FText::Format(INVTEXT("Module {0} was created under {1} successfully!"), Args);
		FMessageDialog::Open(EAppMsgType::Ok, SuccessMessage);
	}
	
	return FReply::Handled();
}

FReply SModuleMakerWidget::HandleCancelButtonClicked()
{
	const TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());

	if(ContainingWindow.IsValid())
	{
		ContainingWindow->DestroyWindowImmediately();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}
