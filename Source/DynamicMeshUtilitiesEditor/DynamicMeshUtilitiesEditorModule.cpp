#include "DynamicMeshUtilitiesEditorModule.h"
#include "Modules/ModuleManager.h"
#include "AssetEditor/SweepMeshAssetActions.h"

#define LOCTEXT_NAMESPACE "DynamicMeshUtilitiesEditorModule"

void FDynamicMeshUtilitiesEditorModule::StartupModule() {
	SweepMeshAssetActions = MakeShared<FSweepMeshAssetActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(SweepMeshAssetActions.ToSharedRef());
}

void FDynamicMeshUtilitiesEditorModule::ShutdownModule() {
	if (FAssetToolsModule::IsModuleLoaded()) FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(SweepMeshAssetActions.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicMeshUtilitiesEditorModule, DynamicMeshUtilitiesEditorMode)