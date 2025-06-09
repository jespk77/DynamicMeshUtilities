#include "SweepMeshAssetEditorToolkit.h"
#include "SweepMeshAssetEditorWidget.h"
#include "SweepMeshAssetEditorCommands.h"
#include "DynamicMeshUtilities/Assets/SweepMeshAsset.h"

const FName FSweepMeshAssetEditorToolkit::SweepAssetViewerId = "SweepAssetViewerTab";
const FName FSweepMeshAssetEditorToolkit::SweepAssetDetailsId = "SweepAssetDetailsTab";

#define LOCTEXT_NAMESPACE "SweepMeshAssetEditor"

void FSweepMeshAssetEditorToolkit::InitSweepMeshEditor(USweepMeshAsset* mesh, TSharedPtr<IToolkitHost> host) {
	SweepMeshAsset = mesh;

	FSweepMeshAssetEditorCommands::Register();
	const TSharedRef<FTabManager::FLayout> layout =
		FTabManager::NewLayout("SweepMeshAssetEditorLayout")
		->AddArea(
		FTabManager::NewPrimaryArea()
		->SetOrientation(EOrientation::Orient_Vertical)
		->Split(
		FTabManager::NewSplitter()
		->SetOrientation(EOrientation::Orient_Horizontal)
		->Split(
		FTabManager::NewStack()
		->SetSizeCoefficient(.7f)
		->SetHideTabWell(true)
		->AddTab(SweepAssetViewerId, ETabState::OpenedTab)
		)
		->Split(
		FTabManager::NewStack()
		->SetSizeCoefficient(.3f)
		->SetHideTabWell(true)
		->AddTab(SweepAssetDetailsId, ETabState::OpenedTab)
		)
		)
		);

	FAssetEditorToolkit::InitAssetEditor(EToolkitMode::Standalone, host, "SweepMeshAssetEditor", layout, true, true, SweepMeshAsset, true);

	AssetEditor->RegisterCommands();
	Extender = MakeShareable(new FExtender);
	Extender->AddToolBarExtension("ViewOptions", EExtensionHook::After, AssetEditor->CommandList, FToolBarExtensionDelegate::CreateRaw(AssetEditor.Get(), &SSweepMeshAssetEditorWidget::CreateToolbar));

	UE_LOG(LogTemp, Log, TEXT("Adding toolbar extender"));
	AddToolbarExtender(Extender);
	RegenerateMenusAndToolbars();
}

void FSweepMeshAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& manager) {
	FAssetEditorToolkit::RegisterTabSpawners(manager);
	WorkspaceMenuCategory = manager->AddLocalWorkspaceMenuCategory(LOCTEXT("SweepMeshEditor", "Sweep Mesh Editor"));

	manager->RegisterTabSpawner(SweepAssetViewerId, FOnSpawnTab::CreateLambda(
		[&](const FSpawnTabArgs& args) {
		return SNew(SDockTab)[
			SAssignNew(AssetEditor, SSweepMeshAssetEditorWidget)
				.SweepMesh(SweepMeshAsset)
		];
	}))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetDisplayName(LOCTEXT("SweepMeshViewer", "Geometry"));

	FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs detailViewArgs;
	detailViewArgs.bAllowSearch = false;
	detailViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	AssetDetails = propertyEditorModule.CreateDetailView(detailViewArgs);
	AssetDetails->SetObject(SweepMeshAsset);

	manager->RegisterTabSpawner(SweepAssetDetailsId, FOnSpawnTab::CreateLambda(
		[&](const FSpawnTabArgs& spawnArgs) {
		return SNew(SDockTab)[
			AssetDetails.ToSharedRef()
		];
	}))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetDisplayName(LOCTEXT("SweepMeshDetails", "Details"));
}

void FSweepMeshAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& manager) {
	FAssetEditorToolkit::UnregisterTabSpawners(manager);
	FSweepMeshAssetEditorCommands::Unregister();
	manager->UnregisterTabSpawner(SweepAssetViewerId);
	manager->UnregisterTabSpawner(SweepAssetDetailsId);
}
#undef LOCTEXT_NAMESPACE
