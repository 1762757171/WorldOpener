// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams( FOnBuildMenu, FMenuBuilder&, const TArray<ULevelStreaming*>&, bool /*IsFolder*/ );

class FWorldOpenerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void OpenWorld();
	TSharedPtr<class FUICommandList> GetCommandList() const;
	WORLDOPENER_API static void OpenWorldWith(const TArray<FString> Levels);
	WORLDOPENER_API static bool IsInTargetWorld();
	WORLDOPENER_API static void LoadLevel(const TArray<FString>& LevelPackageNames, bool OpenLevelIfNotIn = false);

private:
	
	bool OpenWorldCommand_CanExecute() const;
	void OpenWorldCommand_ButtonClicked();

	void RegisterMenus();
	
	TSharedRef<SDockTab> OnSpawnLoaderTab(const FSpawnTabArgs& SpawnTabArgs) const;

	static UWorld* GetPlayWorld();

	static bool IsGamePlaying();

	void OnAssetOpened(UObject* Asset);

	FDelegateHandle OnAssetOpenedHandle;
private:
	
	TSharedPtr<class FUICommandList> PluginCommands;
	TWeakObjectPtr<class ULevelSequence> CurrentLevelSequence;

	static const FName WoLoaderTabId;
public:

	FOnBuildMenu OnBuildMenu;
	
};
