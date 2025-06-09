using UnrealBuildTool;

public class DynamicMeshUtilitiesEditor : ModuleRules {
    public DynamicMeshUtilitiesEditor(ReadOnlyTargetRules target) : base(target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "GeometryCore", "GeometryFramework",
                "DynamicMeshUtilities",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject", "Engine", "UnrealEd",
                "Slate", "SlateCore",
                "InputCore",
                "EditorFramework", "EditorStyle",
                "MeshModelingToolsExp",
            });
    }
}
