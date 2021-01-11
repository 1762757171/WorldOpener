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
	
	bool OpenWorldCommand_CanExecute() const;
	void OpenWorldCommand_ButtonClicked();

	TSharedPtr<class FUICommandList> GetCommandList() const;

private:

	void RegisterMenus();
	
	TSharedRef<SDockTab> OnSpawnLoaderTab(const FSpawnTabArgs& SpawnTabArgs) const;

	static UWorld* GetPlayWorld();

	static bool IsGamePlaying();

private:
	
	TSharedPtr<class FUICommandList> PluginCommands;

	static const FName WoLoaderTabId;

public:

	FOnBuildMenu OnBuildMenu;
	
};
