// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudiscanPlugin.h"

#include "LudiscanPluginStyle.h"
#include "LudiscanPluginCommands.h"
#include "Window/SLudiscanMainWidget.h"

#define LOCTEXT_NAMESPACE "FLudiscanPluginModule"

static const FName LudiscanPluginTabName("FLudiscanPlugin");



void FLudiscanPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FLudiscanPluginStyle::Initialize();
	FLudiscanPluginStyle::ReloadTextures();

	FLudiscanPluginCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FLudiscanPluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLudiscanPluginModule::PluginButtonClicked),
		FCanExecuteAction()
		);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLudiscanPluginModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LudiscanPluginTabName, FOnSpawnTab::CreateRaw(this, &FLudiscanPluginModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("Analytics Editor", "分析Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetIcon(FSlateIcon(FName("CustomWIndowPluginStyle"), FName("CustomWIndowPlugin.OpenPluginWindowIcon")));
}

void FLudiscanPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLudiscanPluginStyle::Shutdown();

	FLudiscanPluginCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LudiscanPluginTabName);
}

void FLudiscanPluginModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LudiscanPluginTabName);
}

void FLudiscanPluginModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLudiscanPluginCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLudiscanPluginCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

TSharedRef<SDockTab> FLudiscanPluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			[
				SNew(SLudiscanMainWidget)
			]
		];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLudiscanPluginModule, LudiscanPlugin)