#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class USweepMeshAsset;

class SSweepMeshAssetEditorWidget : public SLeafWidget {
	private:
		static const float ScrollSensitivity;
		static const FVector2D PointSize;

		enum EDragOperationType {
			None, PanZoom, MovePoint
		};
		EDragOperationType DragType = EDragOperationType::None;

		TAttribute<USweepMeshAsset*> SweepMesh;
		TArray<FVector2D> CachedMeshPoints;
		FTransform2D EditTransform;

		int32 PointIndex = INDEX_NONE;
		int32 AddPoint(const FVector2D& point);
		bool DeleteSelectedPoint();
		int32 FindClosestPoint(const FVector2D& point) const;

		void UpdatePointCache(const bool markDirty=true);

		inline bool IsPointSelected() const { return CachedMeshPoints.IsValidIndex(PointIndex); }

		void ZoomToFit();

	public:
		SLATE_BEGIN_ARGS(SSweepMeshAssetEditorWidget) {}
			SLATE_ATTRIBUTE(USweepMeshAsset*, SweepMesh)
		SLATE_END_ARGS()

		TSharedPtr<FUICommandList> CommandList;
		virtual void RegisterCommands();
		virtual void CreateToolbar(FToolBarBuilder& builder);

		void Construct(const FArguments& args);
		FVector2D ComputeDesiredSize(float scale) const override { return FVector2D(100, 100); }
		int32 OnPaint(const FPaintArgs& args, const FGeometry& geometry, const FSlateRect& rect, FSlateWindowElementList& elements, int32 layerId, const FWidgetStyle& style, bool parentEnabled) const override;

		FReply OnMouseButtonDoubleClick(const FGeometry& geometry, const FPointerEvent& event) override;
		FReply OnMouseButtonDown(const FGeometry& geometry, const FPointerEvent& event) override;
		FReply OnMouseButtonUp(const FGeometry& geometry, const FPointerEvent& event) override;
		FReply OnMouseMove(const FGeometry& geometry, const FPointerEvent& event) override;
		FReply OnMouseWheel(const FGeometry& geometry, const FPointerEvent& event) override;

		bool SupportsKeyboardFocus() const override { return true; }
		FReply OnKeyDown(const FGeometry& geometry, const FKeyEvent& event) override;
};