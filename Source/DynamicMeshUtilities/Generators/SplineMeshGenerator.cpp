#include "SplineMeshGenerator.h"
#include "Components/SplineComponent.h"
#include "Generators/SweepGenerator.h"

bool USplinePathGenerator::AlignLocationWithFloor(FVector& location) const {
	if (!AlignToFloor) return true;

	FHitResult hit;
	const FVector start(location.X, location.Y, location.Z + 300),
		end(location.X, location.Y, location.Z - 300);
	if (GetWorld()->LineTraceSingleByObjectType(hit, start, end, FCollisionObjectQueryParams::AllObjects)) location = hit.Location;
	return true;
}

void USplinePathGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline)) return;

	TArray<FVector> splinePoints;
	Spline->ConvertSplineToPolyLine(ESplineCoordinateSpace::World, MaxDistanceFromSpline, splinePoints);

	using namespace UE::Geometry;
	FGeneralizedCylinderGenerator sweepGenerator;
	bool started = false;
	for (const FVector point : splinePoints) {
		const float key = Spline->FindInputKeyClosestToWorldLocation(point);
		const FQuat rotation = Spline->GetQuaternionAtSplineInputKey(key, SplineSpace);
		FVector location = Spline->GetLocationAtSplineInputKey(key, SplineSpace) + (rotation.GetRightVector() * Offset);
		if (!AlignLocationWithFloor(location)) {
			// because the sweep generator cannot put cuts along a path, cuts can only be placed at the start and end of the path
			// if cuts along a path are desired, this code can be changed to make the sweep generator run multiple times
			if (started) break;
			else continue;
		}

		started = true;
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
