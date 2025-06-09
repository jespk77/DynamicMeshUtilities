#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "SweepMeshAssetEditorToolkit.h"
#include "DynamicMeshUtilities/Assets/SweepMeshAsset.h"

#define LOCTEXT_NAMESPACE "SweepMeshAssetEditor"

class FSweepMeshAssetActions : public FAssetTypeActions_Base {
public:
	UClass* GetSupportedClass() const override { return USweepMeshAsset::StaticClass(); }
	FText GetName() const override { return LOCTEXT("AssetName", "Sweep Mesh"); }
	FColor GetTypeColor() const override { return FColor::Red; }
	uint32 GetCategories() override { return EAssetTypeCategories::Misc; }

	virtual void OpenAssetEditor(const TArray<UObject*>& objects, TSharedPtr<IToolkitHost> host = TSharedPtr<IToolkitHost>()) override {
		for (UObject* object : objects) {
			if (USweepMeshAsset* sweepMesh = Cast<USweepMeshAsset>(object)) {
				MakeShared<FSweepMeshAssetEditorToolkit>()->InitSweepMeshEditor(sweepMesh, host);
			}
		}
	}
};

#undef LOCTEXT_NAMESPACE