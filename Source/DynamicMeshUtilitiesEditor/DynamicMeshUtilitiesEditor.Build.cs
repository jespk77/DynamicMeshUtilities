using UnrealBuildTool;

public class DynamicMeshUtilitiesEditor : ModuleRules {
    public DynamicMeshUtilitiesEditor(ReadOnlyTargetRules target) : base(target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        CppCompileWarningSettings.UnreachableCodeWarningLevel = WarningLevel.Warning;
        CppCompileWarningSettings.ImplicitIntConversionWarningLevel = WarningLevel.Warning;

        PublicDependencyModuleNames.AddRange([
            "Core",
            "GeometryCore", "GeometryFramework",
            "DynamicMeshUtilities",
        ]);

        PrivateDependencyModuleNames.AddRange([
            "CoreUObject", "Engine", "UnrealEd",
            "Slate", "SlateCore",
            "InputCore",
            "EditorFramework", "EditorStyle",
            "MeshModelingToolsExp",
        ]);
    }
}
