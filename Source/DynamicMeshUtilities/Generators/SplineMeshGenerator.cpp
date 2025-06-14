#include "SplineMeshGenerator.h"
#include "Components/SplineComponent.h"
#include "Generators/SweepGenerator.h"

void USplinePathGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline)) return;

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void USplineSurfaceGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline)) return;
}
