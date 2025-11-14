#include "SplineMeshGenerator.h"
#include "Components/SplineComponent.h"

#include "Generators/SweepGenerator.h"
#include "CurveOps/TriangulateCurvesOp.h"

void USplinePathGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline) || Spline->GetNumberOfSplinePoints() == 0 || CrossSection.IsEmpty()) return;

	TArray<FVector> splinePoints;
	Spline->ConvertSplineToPolyLine(ESplineCoordinateSpace::World, MaxDistanceFromSpline, splinePoints);

	using namespace UE::Geometry;
	FGeneralizedCylinderGenerator sweepGenerator;
	for (const FVector& point : splinePoints) {
		const float key = Spline->FindInputKeyClosestToWorldLocation(point);
		const FQuat rotation = Spline->GetQuaternionAtSplineInputKey(key, SplineSpace);
		const FVector location = Spline->GetLocationAtSplineInputKey(key, SplineSpace) + (rotation.GetRightVector() * Offset);

		sweepGenerator.Path.Add(location);
		sweepGenerator.PathFrames.Add(FFrame3d(location, rotation.GetAxisY(), rotation.GetAxisZ(), rotation.GetAxisX()));
		sweepGenerator.PathScales.Add(FVector2D::One());
	}

	// all points have collision so there's nothing to build
	if (sweepGenerator.Path.IsEmpty()) return;

	sweepGenerator.CrossSection.AppendVertices(CrossSection);
	sweepGenerator.bLoop = sweepGenerator.bProfileCurveIsClosed = Spline->IsClosedLoop();
	sweepGenerator.bCapped = !sweepGenerator.bLoop;
	sweepGenerator.CapType = ECapType::FlatTriangulation;
	sweepGenerator.InitialFrame = FFrame3d(sweepGenerator.Path[0]);
	sweepGenerator.bUVScaleRelativeWorld = ScaleUVToWorld;
	sweepGenerator.UnitUVInWorldCoordinates = UVWorldUnit;
	sweepGenerator.Generate();
	mesh.Copy(&sweepGenerator);

	FDynamicMeshNormalOverlay* normals = mesh.Attributes()->GetNormalLayer(0);
	for (int i = 0; i < normals->ElementCount(); i++)
		normals->SetElement(i, FVector3f::UpVector);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void USplineSurfaceGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline) || Spline->GetNumberOfSplinePoints() == 0) return;

	using namespace UE::Geometry;
	FTriangulateCurvesOp curveGenerator;
	curveGenerator.CombineMethod = ECombineCurvesMethod::Union;
	curveGenerator.FlattenMethod = EFlattenCurveMethod::DoNotFlatten;
	curveGenerator.OffsetClosedMethod = EOffsetClosedCurvesMethod::DoNotOffset;
	curveGenerator.OffsetJoinMethod = EOffsetJoinMethod::Round;
	curveGenerator.OffsetOpenMethod = EOffsetOpenCurvesMethod::TreatAsClosed;
	curveGenerator.OpenEndShape = EOpenCurveEndShapes::Round;

	TArray<FVector> splinePoints;
	Spline->ConvertSplineToPolyLine(ESplineCoordinateSpace::World, MaxDistanceFromSpline, splinePoints);
	curveGenerator.AddWorldCurve(splinePoints, true, FTransform::Identity);
	curveGenerator.Thickness = Height;
	curveGenerator.bWorldSpaceUVScale = ScaleUVToWorld;
	curveGenerator.UVScaleFactor = UVWorldUnit;
	curveGenerator.CalculateResult(nullptr);

	mesh.Copy(*curveGenerator.ExtractResult());

	FDynamicMeshNormalOverlay* normals = mesh.Attributes()->GetNormalLayer(0);
	for (int i = 0; i < normals->ElementCount(); i++)
		normals->SetElement(i, FVector3f::UpVector);
}
