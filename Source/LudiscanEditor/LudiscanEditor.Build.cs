using UnrealBuildTool;

public class LudiscanEditor : ModuleRules
{
    public LudiscanEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
                "LudiscanPlugin",
                "OpenAPI",
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                "LudiscanPlugin",
                "OpenAPI",
            }    
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "LudiscanPlugin",
                "OpenAPI",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "ToolWidgets",
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
                "OpenAPI",
                "LudiscanPlugin",
            }
        );
    }
}