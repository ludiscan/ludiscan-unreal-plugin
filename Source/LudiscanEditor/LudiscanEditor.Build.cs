using UnrealBuildTool;

public class LudiscanEditor : ModuleRules
{
    public LudiscanEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "LudiscanPlugin",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "LudiscanPlugin",
                "UnrealEd",
                "Projects",
                "InputCore",
                "EditorFramework",
                "UnrealEd",
                "ToolMenus",
                "Core", 
                "HTTP", "Json", "JsonUtilities", "RHI", "RenderCore",
                "EditorStyle",
                "LevelEditor",
                "InteractiveToolsFramework",
                "EditorInteractiveToolsFramework",
            }
        );
    }
}