// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudiscanPluginCommands.h"

#define LOCTEXT_NAMESPACE "FLudiscanPluginModule"

void FLudiscanPluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "LudiscanPlugin", "Bring up LudiscanPlugin window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
