#include "SweepMeshAssetEditorWidget.h"
#include "DynamicMeshUtilities/Assets/SweepMeshAsset.h"
#include "SweepMeshAssetEditorCommands.h"

const float SSweepMeshAssetEditorWidget::ScrollSensitivity = .2f;
const FVector2D SSweepMeshAssetEditorWidget::PointSize = FVector2D(10);

int32 SSweepMeshAssetEditorWidget::AddPoint(const FVector2D& point) {
	const int32 insertIndex = IsPointSelected() ? PointIndex : CachedMeshPoints.Num(),
		nearestPoint = FindClosestPoint(point);

	if (SweepMesh.IsSet()) {
		const int32 index = SweepMesh.Get()->SweepPoints.Insert(nearestPoint != INDEX_NONE ? CachedMeshPoints[nearestPoint] : EditTransform.Inverse().TransformPoint(point), insertIndex);
		UpdatePointCache();
		return index;
	}

	return INDEX_NONE;
}

bool SSweepMeshAssetEditorWidget::DeleteSelectedPoint() {
	if (IsPointSelected()) {
		if (SweepMesh.IsSet()) {
			SweepMesh.Get()->SweepPoints.RemoveAt(PointIndex);
			UpdatePointCache();
		}
		return true;
	}
	return false;
}

int32 SSweepMeshAssetEditorWidget::FindClosestPoint(const FVector2D& point) const {
	int32 index = INDEX_NONE;
	float indexDistance = INFINITY;
	for (int32 i = 0; i < CachedMeshPoints.Num(); i++) {
		const FVector2D& meshPoint = CachedMeshPoints[i];
		const float distance = (meshPoint - point).Length();
		if (distance < (PointSize.X * 2) && distance < indexDistance) {
			index = i;
			indexDistance = distance;
		}
	}
	return index;
}

void SSweepMeshAssetEditorWidget::UpdatePointCache(const bool markDirty) {
	if (SweepMesh.IsSet()) {
		USweepMeshAsset* sweepMesh = SweepMesh.Get();
		CachedMeshPoints.Reset(sweepMesh->SweepPoints.Num());
		for (const FVector2D& point : sweepMesh->SweepPoints)
			CachedMeshPoints.Add(EditTransform.TransformPoint(point));

		if (markDirty) {
			sweepMesh->MarkPackageDirty();
			sweepMesh->OnAssetEdited.Broadcast();
		}
	}
	else CachedMeshPoints.Reset();
}

void SSweepMeshAssetEditorWidget::ZoomToFit() {

}

void SSweepMeshAssetEditorWidget::RegisterCommands() {
	UE_LOG(LogTemp, Log, TEXT("Registering widget commands"));
	const FSweepMeshAssetEditorCommands& commands = FSweepMeshAssetEditorCommands::Get();
	CommandList->MapAction(commands.CommandZoomToFit, FExecuteAction::CreateRaw(this, &SSweepMeshAssetEditorWidget::ZoomToFit));
}

void SSweepMeshAssetEditorWidget::CreateToolbar(FToolBarBuilder& builder) {
	UE_LOG(LogTemp, Log, TEXT("Adding toolbar commands"));
	const FSweepMeshAssetEditorCommands& commands = FSweepMeshAssetEditorCommands::Get();
	builder.BeginSection("View");
	builder.AddToolBarButton(commands.CommandZoomToFit);
	builder.EndSection();
}

void SSweepMeshAssetEditorWidget::Construct(const FArguments& args) {
	EditTransform = FTransform2D(FScale2f(-1), FVector2D::ZeroVector);
	SweepMesh = args._SweepMesh;
	CommandList = MakeShareable(new FUICommandList);
	UpdatePointCache(false);
	ZoomToFit();
}

int32 SSweepMeshAssetEditorWidget::OnPaint(const FPaintArgs& args, const FGeometry& geometry, const FSlateRect& rect, FSlateWindowElementList& elements, int32 layerId, const FWidgetStyle& style, bool parentEnabled) const {
	static const FColor SelectedColor = FColor::White;
	static const FColor PointColor = FColor(125, 125, 125);

	if (!CachedMeshPoints.IsEmpty()) {
		for (int32 i = 0; i < CachedMeshPoints.Num(); i++) {
			const FVector2D meshPoint = CachedMeshPoints[i];
			FSlateDrawElement::MakeBox(elements, layerId + 1,
				geometry.ToPaintGeometry(PointSize, FSlateLayoutTransform(meshPoint - (PointSize / 2))),
				FAppStyle::Get().GetBrush("Sequencer.KeyDiamond"),
				ESlateDrawEffect::None,
				i == PointIndex ? SelectedColor : PointColor);
		}
		FSlateDrawElement::MakeLines(elements, layerId, geometry.ToPaintGeometry(),
			CachedMeshPoints, ESlateDrawEffect::None, FColor::Cyan, true, 2);
	}
	else FSlateDrawElement::MakeText(elements, layerId, geometry.ToPaintGeometry(),
		INVTEXT("Add points to make a shape"),
		FCoreStyle::Get().GetFontStyle("PropertyWindow.NormalFont"));
	return layerId;
}

FReply SSweepMeshAssetEditorWidget::OnMouseButtonDoubleClick(const FGeometry& geometry, const FPointerEvent& event) {
	if (event.GetEffectingButton() == EKeys::LeftMouseButton) {
		AddPoint(geometry.AbsoluteToLocal(event.GetScreenSpacePosition()));
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SSweepMeshAssetEditorWidget::OnMouseButtonDown(const FGeometry& geometry, const FPointerEvent& event) {
	if (event.GetEffectingButton() == EKeys::LeftMouseButton) {
		const FVector2D clickedPosition = geometry.AbsoluteToLocal(event.GetScreenSpacePosition());
		if (IsPointSelected() && event.IsShiftDown()) {
			AddPoint(clickedPosition);
			return FReply::Handled();
		}

		const int32 previousPoint = PointIndex;
		PointIndex = FindClosestPoint(clickedPosition);
		// find a better alternative, gets annoying quickly but without it points get moved unintentionally...
		//if (previousPoint != PointIndex) 
		//	// prevent immediately going to move point mode if a different point was selected
		//	return FReply::Handled();

		if (IsPointSelected()) {
			DragType = EDragOperationType::MovePoint;
			return FReply::Handled().CaptureMouse(SharedThis(this));
		}
	}
	else if (event.GetEffectingButton() == EKeys::RightMouseButton) {
		DragType = EDragOperationType::PanZoom;
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}

	DragType = EDragOperationType::None;
	return FReply::Unhandled();
}

FReply SSweepMeshAssetEditorWidget::OnMouseButtonUp(const FGeometry& geometry, const FPointerEvent& event) {
	switch (DragType) {
		case PanZoom:
			return FReply::Handled().ReleaseMouseCapture();
		case MovePoint:
		{
			if (IsPointSelected()) {
				USweepMeshAsset* sweepMesh = SweepMesh.Get();
				sweepMesh->SweepPoints[PointIndex] = EditTransform.Inverse().TransformPoint(CachedMeshPoints[PointIndex]);
				sweepMesh->MarkPackageDirty();
				UpdatePointCache();
			}
			return FReply::Handled().ReleaseMouseCapture();
		}
		default: return FReply::Unhandled();
	}
}

FReply SSweepMeshAssetEditorWidget::OnMouseMove(const FGeometry& geometry, const FPointerEvent& event) {
	if (!HasMouseCapture()) return FReply::Unhandled();

	switch (DragType) {
		case PanZoom:
		{
			FVector2D translation = EditTransform.GetTranslation();
			translation += event.GetCursorDelta();
			EditTransform.SetTranslation(translation);
			UpdatePointCache(false);
			return FReply::Handled();
		}
		case MovePoint:
		{
			FVector2D& point = CachedMeshPoints[PointIndex];
			const FVector2D delta = event.GetCursorDelta();
			point.X += FMath::RoundToInt(delta.X);
			point.Y += FMath::RoundToInt(delta.Y);
			return FReply::Handled();
		}
		default: return FReply::Unhandled();
	}
}

FReply SSweepMeshAssetEditorWidget::OnMouseWheel(const FGeometry& geometry, const FPointerEvent& event) {
	const float delta = event.GetWheelDelta() * ScrollSensitivity;
	const FVector2D& translation = EditTransform.GetTranslation();
	float scale = EditTransform.GetMatrix().GetScale().GetVector().X; // scale is uniform so X == Y
	scale = FMath::Clamp(scale + delta, 1.f, 5.f);
	EditTransform = FTransform2D(FScale2f(-scale), translation);
	UpdatePointCache(false);
	return FReply::Handled();
}

FReply SSweepMeshAssetEditorWidget::OnKeyDown(const FGeometry& geometry, const FKeyEvent& event) {
	if (event.GetKey() == EKeys::Delete) {
		DeleteSelectedPoint();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
