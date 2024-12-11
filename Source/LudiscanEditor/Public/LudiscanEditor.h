#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLudiscanEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
private:
    void PluginButtonClicked();
    void RegisterMenus();

    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);

    TSharedPtr<class FUICommandList> PluginCommands = nullptr;
};
