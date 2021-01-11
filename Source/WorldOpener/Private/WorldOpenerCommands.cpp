// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldOpenerCommands.h"

#define LOCTEXT_NAMESPACE "FWorldOpenerModule"

void FWoCommands::RegisterCommands()
{
	UI_COMMAND(Action_OpenWorld, "WorldOpener", "Open world with WorldOpener.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
