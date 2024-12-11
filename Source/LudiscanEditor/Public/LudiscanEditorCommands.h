// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LudiscanEditorStyle.h"

class FLudiscanEditorCommands : public TCommands<FLudiscanEditorCommands>
{
public:

	FLudiscanEditorCommands()
		: TCommands<FLudiscanEditorCommands>(
		TEXT("LudiscanPlugin"),
			NSLOCTEXT("Contexts", "LudiscanPlugin", "LudiscanPlugin"),
			NAME_None, FLudiscanEditorStyle::GetStyleSetName()) {
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> InteractiveTool;

	TSharedPtr< FUICommandInfo > OpenPluginWindow;
protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};