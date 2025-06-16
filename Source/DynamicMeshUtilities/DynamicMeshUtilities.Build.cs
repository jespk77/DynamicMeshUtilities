using UnrealBuildTool;

public class DynamicMeshUtilities : ModuleRules {
    public DynamicMeshUtilities(ReadOnlyTargetRules target) : base(target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "GeometryCore", "GeometryFramework",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject", "Engine",
                "Slate", "SlateCore",
                "MeshModelingToolsExp",
                "ModelingOperators",
            });
    }
}
