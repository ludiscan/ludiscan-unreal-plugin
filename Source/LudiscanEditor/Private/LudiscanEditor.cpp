#include "LudiscanEditor.h"

#include "LudiscanEditorCommands.h"
#include "LudiscanEditorStyle.h"
#include "Window/SLudiscanMainWidget.h"

#define LOCTEXT_NAMESPACE "FLudiscanEditorModule"

static const FName LudiscanPluginTabName("FLudiscanPlugin");

void FLudiscanEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FLudiscanEditorStyle::Initialize();
	FLudiscanEditorStyle::ReloadTextures();

	FLudiscanEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FLudiscanEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLudiscanEditorModule::PluginButtonClicked),
		FCanExecuteAction()
		);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLudiscanEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LudiscanPluginTabName, FOnSpawnTab::CreateRaw(this, &FLudiscanEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("Analytics Editor", "分析Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetIcon(FSlateIcon(FName("CustomWIndowPluginStyle"), FName("CustomWIndowPlugin.OpenPluginWindowIcon")));
}

void FLudiscanEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLudiscanEditorStyle::Shutdown();

	FLudiscanEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LudiscanPluginTabName);
}

void FLudiscanEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LudiscanPluginTabName);
}

void FLudiscanEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLudiscanEditorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLudiscanEditorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

TSharedRef<SDockTab> FLudiscanEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
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
    
IMPLEMENT_MODULE(FLudiscanEditorModule, LudiscanEditor)