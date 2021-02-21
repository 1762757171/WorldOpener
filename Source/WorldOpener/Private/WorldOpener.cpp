// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldOpener.h"

#include "ContentBrowserMenuContexts.h"
#include "FileHelpers.h"
#include "LevelSequence.h"
#include "WorldOpenerStyle.h"
#include "WorldOpenerCommands.h"
#include "ToolMenus.h"
#include "WorldOpenerSettings.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorldOpenerUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
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
		[this](FMenuBuilder& InMenuBuilder, const TArray<ULevelStreaming*>& InLevelStreamings, bool bIsFolder)
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

				auto AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
				if(CurrentLevelSequence.IsValid() && AssetEditor->FindEditorForAsset(CurrentLevelSequence.Get(), false))
				{
	                InMenuBuilder.AddMenuEntry(
	                    LOCTEXT("Label_MarkToSequence", "MarkToSequence"),
	                    LOCTEXT("Tooltip_MarkToSequence", "Mark level(s) associated to current sequence"),
	                    FSlateIcon(),
	                    FUIAction(FExecuteAction::CreateLambda
	                    (
	                    [this, InLevelStreamings]
	                        {
								auto Setting = GetMutableDefault<UWoSettings>();
	                    		TSoftObjectPtr<ULevelSequence> Ref(CurrentLevelSequence.Get());
	                    		auto& Maps = Setting->SequenceAdditionAssociatedWorld.FindOrAdd(Ref);
					            for(const auto& l : InLevelStreamings)
					            	Maps.Maps.AddUnique(l->GetWorldAsset());
	                    		Setting->Modify();
								Setting->SaveConfig();
	                        }
	                    ))
	                );
				}
			}
			InMenuBuilder.EndSection();
		}
	);

	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
	    auto AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	    OnAssetOpenedHandle = AssetEditor->OnAssetEditorOpened().AddRaw(this, &FWorldOpenerModule::OnAssetOpened);
	});
}

void FWorldOpenerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWoStyle::Shutdown();

	FWoCommands::Unregister();
	//auto AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	//AssetEditor->OnAssetEditorOpened().Remove(OnAssetOpenedHandle);
}

bool FWorldOpenerModule::OpenWorldCommand_CanExecute() const
{
	return !IsGamePlaying();
}

void FWorldOpenerModule::OpenWorld()
{
	OpenWorldWith({});
}

void FWorldOpenerModule::OpenWorldCommand_ButtonClicked()
{
	OpenWorld();
}

TSharedPtr<FUICommandList> FWorldOpenerModule::GetCommandList() const
{
	return PluginCommands;
}

void FWorldOpenerModule::OpenWorldWith(const TArray<FString> Levels)
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
	
	if(IsInTargetWorld())
	{
		const FText Format = LOCTEXT("Prompt_AlreadyIn", "You are already in level {0}!");
		const FText Prompt = FText::FormatOrdered(Format, FText::FromString( WorldAsset.GetAssetName() ));
		FMessageDialog::Open(EAppMsgType::Ok, Prompt);
	}
	else
	{
		UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
		
		if (FEditorFileUtils::SaveDirtyPackages(true, true, true) == false)
		{
			// The user pressed cancel. Abort the import so the user doesn't lose any changes
			return;
		}

		const TSet<TAssetPtr<UWorld>>& WorldsPreload = GetMutableDefault<UWoSettings>()->PreloadWorlds;
	
		for(auto l : WorldToLoad->GetStreamingLevels())
		{
			if(!WorldsPreload.Contains( l->GetWorldAsset() ) && !Levels.Contains(l->GetWorldAssetPackageName()))
			{
				auto& CurrentState = access_private::CurrentState(*l);
				CurrentState = ULevelStreaming::ECurrentState::FailedToLoad;
			}
		}
	
		FEditorFileUtils::LoadMap(WorldAsset.ToString(), false, true);
	}

	FGlobalTabmanager::Get()->TryInvokeTab(WoLoaderTabId);
}

bool FWorldOpenerModule::IsInTargetWorld()
{
	const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;
	UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
	
	return GEditor->GetEditorWorldContext().World() == WorldToLoad;
}

void FWorldOpenerModule::LoadLevel(const TArray<FString>& LevelPackageNames, bool OpenLevelIfNotIn)
{
	const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;
	UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
	if(GEditor->GetEditorWorldContext().World() != WorldToLoad)
	{
		if(OpenLevelIfNotIn)
			OpenWorldWith(LevelPackageNames);
		return;
	}
	TArray<ULevelStreaming*> Streamings;
	for(auto l : WorldToLoad->GetStreamingLevels())
	{
		if(LevelPackageNames.Contains(l->GetWorldAssetPackageName()))
		{
			Streamings.Add(l);
			WoUtils::LoadLevel(l);
		}
	}
	WoUtils::FlushWorld(Streamings);
}

static void OpenLevels(const TArray<FString> AssociatedMaps)
{
	const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;
	UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
	if(GEditor->GetEditorWorldContext().World() != WorldToLoad)
		return;
	TArray<ULevelStreaming*> Streamings;
	for(auto l : WorldToLoad->GetStreamingLevels())
	{
		if(AssociatedMaps.Contains(l->GetWorldAssetPackageName()))
		{
			Streamings.Add(l);
			WoUtils::LoadLevel(l);
		}
	}
	WoUtils::FlushWorld(Streamings);
}

void RecursiveGetDependencies(const FName& PackageName, TArray<FAssetIdentifier>& AllDependencies)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FName> Dependencies;
	AssetRegistryModule.Get().GetDependencies(PackageName, Dependencies);

	for (auto DependsIt = Dependencies.CreateConstIterator(); DependsIt; ++DependsIt)
	{
		if (!AllDependencies.Contains(*DependsIt))
		{
			// @todo Make this skip all packages whose root is different than the source package list root. For now we just skip engine content.
			//const bool bIsEnginePackage = (*DependsIt).ToString().StartsWith(TEXT("/Engine"));
			//const bool bIsScriptPackage = (*DependsIt).ToString().StartsWith(TEXT("/Script"));
			//const bool bIsNiagaraPackage = (*DependsIt).ToString().StartsWith(TEXT("/Niagara"));
			const bool bIsGamePackage = (*DependsIt).ToString().StartsWith(TEXT("/Game"));
			const bool bIsNotMainWorld = GetMutableDefault<UWoSettings>()->WorldToOpen.GetLongPackageName() != DependsIt->ToString();
			//if (!bIsEnginePackage && !bIsScriptPackage && !bIsNiagaraPackage)
			if (bIsGamePackage && bIsNotMainWorld)
			{
				AllDependencies.Add(*DependsIt);
				RecursiveGetDependencies(*DependsIt, AllDependencies);
			}
		}
	}
}

void RecursiveGetReferencers(const FName& PackageName, TArray<FAssetIdentifier>& AllReferencers)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FName> Referencers;
	AssetRegistryModule.Get().GetReferencers(PackageName, Referencers);

	for (auto DependsIt = Referencers.CreateConstIterator(); DependsIt; ++DependsIt)
	{
		if (!AllReferencers.Contains(*DependsIt))
		{
			// @todo Make this skip all packages whose root is different than the source package list root. For now we just skip engine content.
			//const bool bIsEnginePackage = (*DependsIt).ToString().StartsWith(TEXT("/Engine"));
			//const bool bIsScriptPackage = (*DependsIt).ToString().StartsWith(TEXT("/Script"));
			//const bool bIsNiagaraPackage = (*DependsIt).ToString().StartsWith(TEXT("/Niagara"));
			const bool bIsGamePackage = (*DependsIt).ToString().StartsWith(TEXT("/Game"));
			const bool bIsNotMainWorld = GetMutableDefault<UWoSettings>()->WorldToOpen.GetLongPackageName() != DependsIt->ToString();
			//if (!bIsEnginePackage && !bIsScriptPackage && !bIsNiagaraPackage)
			if (bIsGamePackage && bIsNotMainWorld)
			{
				AllReferencers.Add(*DependsIt);
				RecursiveGetDependencies(*DependsIt, AllReferencers);
			}
		}
	}
}

TArray<FString> GetSequenceAssociatedMaps(ULevelSequence* LevelSequence)
{
	// if this LevelSequence has associated maps, offer to load them

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FName LSMapPathName = *LevelSequence->GetOutermost()->GetPathName();

	TArray<FString> AssociatedMaps;

	TArray<FAssetIdentifier> AssociatedAssets;
	auto Setting = GetMutableDefault<UWoSettings>();
	// This makes the assumption these functions will append the array, and not clear it.
	RecursiveGetReferencers(LSMapPathName, AssociatedAssets);
	RecursiveGetDependencies(LSMapPathName, AssociatedAssets);

	for (FAssetIdentifier& AssociatedMap : AssociatedAssets)
	{
		FString MapFilePath;
		FString LevelPath = AssociatedMap.PackageName.ToString();
		if (FEditorFileUtils::IsMapPackageAsset(LevelPath, MapFilePath))
		{
			AssociatedMaps.AddUnique(LevelPath);
		}
		auto Name = FPaths::GetBaseFilename(AssociatedMap.ToString());
		FAssetData AssetData = AssetRegistryModule.GetRegistry().GetAssetByObjectPath(*FString::Printf(TEXT("%s.%s"), *AssociatedMap.ToString(), *Name));
		if(AssetData.AssetClass == "LevelSequence")
		{
			TAssetPtr<ULevelSequence> Ref(AssetData.ToSoftObjectPath());
			if(auto Ptr = Setting->SequenceAdditionAssociatedWorld.Find(Ref))
			{
				for(auto& Map : Ptr->Maps)
					AssociatedMaps.AddUnique(Map.GetLongPackageName());
			};
		}
	}
	TAssetPtr<ULevelSequence> Ref(LevelSequence);
	if(auto Ptr = Setting->SequenceAdditionAssociatedWorld.Find(Ref))
	{
		for(auto& Map : Ptr->Maps)
			AssociatedMaps.AddUnique(Map.GetLongPackageName());
	};
	return AssociatedMaps;
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

	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		UToolMenu* Menu = ToolMenus->ExtendMenu("ContentBrowser.AssetContextMenu.LevelSequence");
		if (!Menu)
		{
			return;
		}

		FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
		Section.AddDynamicEntry("SequencerActions_WorldOpener", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
			if (!Context)
			{
				return;
			}

			ULevelSequence* LevelSequence = Context->SelectedObjects.Num() == 1 ? Cast<ULevelSequence>(Context->SelectedObjects[0]) : nullptr;
			if (LevelSequence)
			{
				auto AssociatedMaps = GetSequenceAssociatedMaps(LevelSequence);
				if(AssociatedMaps.Num()>0)
				{
                    InSection.AddMenuEntry(
                        "SequencerOpenMap_Label",
                        LOCTEXT("SequencerOpenMap_Label", "Open Map（World Opener)"),
                        LOCTEXT("SequencerOpenMap_Tooltip", "Open a map associated with this Level Sequence Asset"),
                        FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Levels"),
                        FExecuteAction::CreateLambda(
                        [AssociatedMaps]
                        {
					        const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;
					        UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
					        if(GEditor->GetEditorWorldContext().World() != WorldToLoad)
        						OpenWorldWith(AssociatedMaps);
                        	else
                        		OpenLevels(AssociatedMaps);
                        }));
                }
			}
		}));
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

void FWorldOpenerModule::OnAssetOpened(UObject* Asset)
{
	if(auto LevelSequence = Cast<ULevelSequence>(Asset))
	{
		CurrentLevelSequence = LevelSequence;
		const TAssetPtr<UWorld> WorldAsset = GetMutableDefault<UWoSettings>()->WorldToOpen;
		UWorld* WorldToLoad = WorldAsset.LoadSynchronous();
		if(GEditor->GetEditorWorldContext().World() != WorldToLoad)
			return;
		
		auto AssociatedMaps = GetSequenceAssociatedMaps(LevelSequence);
		TArray<ULevelStreaming*> Streamings;
		for(auto l : WorldToLoad->GetStreamingLevels())
		{
			if(AssociatedMaps.Contains(l->GetWorldAssetPackageName()))
			{
				Streamings.Add(l);
				WoUtils::LoadLevel(l);
			}
		}
		WoUtils::FlushWorld(Streamings);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldOpenerModule, WorldOpener)