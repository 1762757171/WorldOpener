// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "WorldOpenerStyle.h"

class FWoCommands : public TCommands<FWoCommands>
{
public:

	FWoCommands()
		: TCommands<FWoCommands>(TEXT("WorldOpener"), NSLOCTEXT("Contexts", "WorldOpener", "WorldOpener Plugin"), NAME_None, FWoStyle::GetStyleSetName())
	{}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > Action_OpenWorld;
};
