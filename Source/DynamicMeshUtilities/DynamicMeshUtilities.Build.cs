using UnrealBuildTool;

public class DynamicMeshUtilities : ModuleRules {
    public DynamicMeshUtilities(ReadOnlyTargetRules target) : base(target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange([
            "Core",
            "DynamicMesh",
            "GeometryCore", "GeometryFramework",
        ]);

        PrivateDependencyModuleNames.AddRange([
            "CoreUObject", "Engine",
            "MeshModelingToolsExp",
            "ModelingOperators",
            "GeometryAlgorithms",
        ]);
    }
}
