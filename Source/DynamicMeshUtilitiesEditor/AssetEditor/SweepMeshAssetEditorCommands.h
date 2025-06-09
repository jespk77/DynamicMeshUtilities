#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "SweepMeshAssetEditor"

class FSweepMeshAssetEditorCommands : public TCommands<FSweepMeshAssetEditorCommands> {
	public:
		FSweepMeshAssetEditorCommands() :
			TCommands<FSweepMeshAssetEditorCommands>(
				TEXT("SweepMeshEditor"), 
				LOCTEXT("SweepMeshEditor", "Sweep Mesh Editor"),
				NAME_None,
				FAppStyle::GetAppStyleSetName()
			){ }

		TSharedPtr<FUICommandInfo> CommandZoomToFit;

		virtual void RegisterCommands() override {
			UE_LOG(LogTemp, Log, TEXT("Registering ui commands"));
			UI_COMMAND(CommandZoomToFit, "Zoom To Fit", "Zoom To Fit 222", EUserInterfaceActionType::Button, FInputChord());
		}
};

#undef LOCTEXT_NAMESPACE
