// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LudiscanPluginStyle.h"

class FLudiscanPluginCommands : public TCommands<FLudiscanPluginCommands>
{
public:

	FLudiscanPluginCommands()
		: TCommands<FLudiscanPluginCommands>(
		TEXT("LudiscanPlugin"),
			NSLOCTEXT("Contexts", "LudiscanPlugin", "LudiscanPlugin"),
			NAME_None, FLudiscanPluginStyle::GetStyleSetName()) {
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> InteractiveTool;

	TSharedPtr< FUICommandInfo > OpenPluginWindow;
protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};