#pragma once

class FDynamicMeshUtilitiesModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
