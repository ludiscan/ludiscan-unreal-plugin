// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudiscanPluginCommands.h"

#define LOCTEXT_NAMESPACE "FLudiscanPluginModule"

void FLudiscanPluginCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);
	UI_COMMAND(OpenPluginWindow, "LudiscanPlugin", "Bring up LudiscanPlugin window", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(OpenPluginWindow);

	UI_COMMAND(InteractiveTool, "HeatMap", "Measure distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FLudiscanPluginCommands::GetCommands()
{
	return FLudiscanPluginCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
