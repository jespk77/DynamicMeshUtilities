#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class USweepMeshAsset;
class SSweepMeshAssetEditorWidget;

#define LOCTEXT_NAMESPACE "SweepMeshAssetEditor"

class FSweepMeshAssetEditorToolkit : public FAssetEditorToolkit {
	private:
		USweepMeshAsset* SweepMeshAsset;
		TSharedPtr<SSweepMeshAssetEditorWidget> AssetEditor;
		TSharedPtr<IDetailsView> AssetDetails;
		TSharedPtr<FExtender> Extender;

		static const FName SweepAssetViewerId, SweepAssetDetailsId;

	public:
		virtual void InitSweepMeshEditor(USweepMeshAsset* mesh, TSharedPtr<IToolkitHost> host);

		virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& manager) override;
		virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& manager) override;

		FName GetToolkitFName() const override { return "SweepMeshAssetEditor"; }
		FText GetBaseToolkitName() const override { return LOCTEXT("SweepMeshEditorName", "Sweep Mesh Editor"); }
		FString GetWorldCentricTabPrefix() const override { return "Sweep Mesh "; }
		FLinearColor GetWorldCentricTabColorScale() const override { return FColor::Blue; }
};

#undef LOCTEXT_NAMESPACE
