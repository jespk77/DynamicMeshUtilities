#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSweepMeshAssetActions;

class FDynamicMeshUtilitiesEditorModule : public IModuleInterface {
private:
	TSharedPtr<FSweepMeshAssetActions> SweepMeshAssetActions;

public:
	/// IModuleInterface implementation ///

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
