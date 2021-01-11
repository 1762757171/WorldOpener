// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldOpener.h"
#include "FileHelpers.h"
#include "WorldOpenerStyle.h"
#include "WorldOpenerCommands.h"
#include "ToolMenus.h"
#include "WorldOpenerSettings.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorldOpenerUtils.h"
#include "Slate/SWorldOpener.h"

static const FName WorldOpenerTabName("WorldOpener");

#define LOCTEXT_NAMESPACE "FWorldOpenerModule"

const FName FWorldOpenerModule::WoLoaderTabId("WoLoader");

void FWorldOpenerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FWoStyle::Initialize();
	FWoStyle::ReloadTextures();

	FWoCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction
	(
		FWoCommands::Get().Action_OpenWorld,
		FExecuteAction::CreateRaw(this, &FWorldOpenerModule::OpenWorldCommand_ButtonClicked),
		FCanExecuteAction::CreateRaw(this, &FWorldOpenerModule::OpenWorldCommand_CanExecute)
	);
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WoLoaderTabId, FOnSpawnTab::CreateRaw(this, &FWorldOpenerModule::OnSpawnLoaderTab))
        .SetDisplayName(NSLOCTEXT("FMapManagerModule", "LoaderTabTitle", "Level Loader"))
        .SetTooltipText(NSLOCTEXT("FMapManagerModule", "LevelLoaderTooltipText", "Open the Level Loader tab."))
        .SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
        .SetIcon(FSlateIcon(FWoStyle::GetStyleSetName(), "WorldOpener.Action_OpenWorld_Small"))
    ;

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWorldOpenerModule::RegisterMenus));

	OnBuildMenu.AddLambda
	(
		[](FMenuBuilder& InMenuBuilder, const TArray<ULevelStreaming*>& InLevelStreamings, bool bIsFolder)
		{
			InMenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_Level", "Level"));
			{
				if(InLevelStreamings.Num() == 1 && !bIsFolder)
				{
					ULevelStreaming* LevelStreaming = InLevelStreamings[0];
					InMenuBuilder.AddMenuEntry
					(
                     LOCTEXT("Label_MakeCurrent", "Make current"),
                     LOCTEXT("Tooltip_MakeCurrent", "Load this level and make it current"),
                     FSlateIcon(),
                     FUIAction(FExecuteAction::CreateLambda
                         (
                             [LevelStreaming]
                             {
                                 WoUtils::LoadLevel(LevelStreaming);
                                 WoUtils::FlushWorld({LevelStreaming});
                                 WoUtils::MakeCurrent(LevelStreaming);
                             }
                         ))
					);
				}

				bool bAnyLoaded = false;
				for(const auto& l : InLevelStreamings)
				{
					if(l->HasLoadedLevel())
					{
                        bAnyLoaded = true;
                        break;
                    }
				}
				
					InMenuBuilder.AddMenuEntry
					(
					bAnyLoaded ? LOCTEXT("Label_Unload", "Unload") : LOCTEXT("Label_Load", "Load"),
					bAnyLoaded ? LOCTEXT("Tooltip_Unload", "Unload level(s)") : LOCTEXT("Tooltip_Load", "Load level(s)"),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda
					(
					[InLevelStreamings, bAnyLoaded]
						{
					        if(bAnyLoaded)
					            for(const auto& l : InLevelStreamings)
					                WoUtils::UnloadLevel(l);
					        else
					            for(const auto& l : InLevelStreamings)
					                 WoUtils::LoadLevel(l);
					        WoUtils::FlushWorld(InLevelStreamings);
                        }
                     ))
                );
			}
			InMenuBuilder.EndSection();
		}
	);
}

void FWorldOpenerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWoStyle::Shutdown();

	FWoCommands::Unregister();
}

bool FWorldOpenerModule::OpenWorldCommand_CanExecute() const
{
	return !IsGamePlaying();
}

void FWorldOpenerModule::OpenWorldCommand_ButtonClicked()
{
	if(IsGamePlaying())
	{
		return;
	}

	const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;

	if (WorldAsset.IsNull())
	{
		FMessageDialog::Open(
			EAppMsgType::Ok,
			LOCTEXT("Prompt_NoWorld", "Please select a level to open by clicking the drop down arrow beside the Open World button in the level editor toolbar."))
		;
		return;
	}
	
	UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
	
	if(GEditor->GetEditorWorldContext().World() == WorldToLoad)
	{
		const FText Format = LOCTEXT("Prompt_AlreadyIn", "You are already in level {0}!");
		const FText Prompt = FText::FormatOrdered(Format, FText::FromString( WorldAsset.GetAssetName() ));
		FMessageDialog::Open(EAppMsgType::Ok, Prompt);
	}
	else
	{
		if (FEditorFileUtils::SaveDirtyPackages(true, true, true) == false)
		{
			// The user pressed cancel. Abort the import so the user doesn't lose any changes
			return;
		}

		const TSet<TAssetPtr<UWorld>>& WorldsPreload = GetMutableDefault<UWoSettings>()->PreloadWorlds;
	
		for(auto l : WorldToLoad->GetStreamingLevels())
		{
			if(!WorldsPreload.Contains( l->GetWorldAsset() ))
			{
				auto& CurrentState = access_private::CurrentState(*l);
				CurrentState = ULevelStreaming::ECurrentState::FailedToLoad;
			}
		}
	
		FEditorFileUtils::LoadMap(WorldAsset.ToString(), false, true);
	}

	FGlobalTabmanager::Get()->TryInvokeTab(WoLoaderTabId);
}

TSharedPtr<FUICommandList> FWorldOpenerModule::GetCommandList() const
{
	return PluginCommands;
}

void FWorldOpenerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWoCommands::Get().Action_OpenWorld, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Content");
			{
				FToolMenuEntry& Entry =
					Section.AddEntry(
						FToolMenuEntry::InitToolBarButton(
							FWoCommands::Get().Action_OpenWorld,
							LOCTEXT("OpenWorldLabel", "Open World"),
							LOCTEXT("OpenWorldTooltip", "Open the world.")
				));
				Entry.SetCommandList(PluginCommands);

				FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
				UWoSettings* Settings = GetMutableDefault<UWoSettings>();
				FDetailsViewArgs DetailsViewArgs;
				DetailsViewArgs.bUpdatesFromSelection = false;
				DetailsViewArgs.bLockable = false;
				DetailsViewArgs.bAllowSearch = false;
				DetailsViewArgs.bAllowFavoriteSystem = false;
				DetailsViewArgs.bShowOptions = false;
				DetailsViewArgs.bShowPropertyMatrixButton = false;
				DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
				TSharedRef<IDetailsView> DetailView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
				DetailView->SetObject(Settings);
				DetailView->OnFinishedChangingProperties().AddLambda
				(
					[](const FPropertyChangedEvent&)
					{
					    UWoSettings* PlaySettings = GetMutableDefault<UWoSettings>();
					    PlaySettings->SaveConfig();
					}
				);
				
				Section.AddEntry(
					FToolMenuEntry::InitComboButton(
                    "WorldOpenerCombo",
                    FToolUIActionChoice(),
                    FOnGetContent::CreateLambda
                    (
						[DetailView]
						{
							return
							SNew(SVerticalBox)
								+SVerticalBox::Slot()
								[
									DetailView
								]
							;
						}
                    )
                ));
			}
		}
	}
}

TSharedRef<SDockTab> FWorldOpenerModule::OnSpawnLoaderTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	const TSharedRef<SDockTab> Tab =
		SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
	;

	const TSharedPtr<SWidget> TabContent =
        SNew(SWorldOpener)
			.IsEnabled_Lambda
			(
				[]
				{
					return !IsGamePlaying();
				}
			)
    ;

	Tab->SetContent(TabContent.ToSharedRef());

	return Tab;
}

UWorld* FWorldOpenerModule::GetPlayWorld()
{
	return GEditor->PlayWorld;
}

bool FWorldOpenerModule::IsGamePlaying()
{
	return GetPlayWorld() != nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldOpenerModule, WorldOpener)