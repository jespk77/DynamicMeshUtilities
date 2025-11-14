#include "SplineMeshGenerator.h"
#include "Components/SplineComponent.h"

#include "Generators/SweepGenerator.h"
#include "CurveOps/TriangulateCurvesOp.h"
#include "ConstrainedDelaunay2.h"
#include "Generators/FlatTriangulationMeshGenerator.h"
#include "Operations/ExtrudeMesh.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////

void USplinePolygonSurfaceGenerator::Generate(FDynamicMesh3& mesh) {
	if (!ensure(Spline) || Spline->GetNumberOfSplinePoints() == 0) return;

	using namespace UE::Geometry;
	TArray<FVector> splinePoints;
	Spline->ConvertSplineToPolyLine(ESplineCoordinateSpace::World, MaxDistanceFromSpline, splinePoints);

	FPolygon2d splinePolygon;
	for (const FVector& point : splinePoints)
		splinePolygon.AppendVertex(FVector2D(point));

	FConstrainedDelaunay2d triangulator;
	triangulator.Add(splinePolygon);
	triangulator.Triangulate();
	if (triangulator.Triangles.IsEmpty()) return;

	FFlatTriangulationMeshGenerator generator;
	generator.Vertices2D = triangulator.Vertices;
	generator.Triangles2D = triangulator.Triangles;
	mesh.Copy(&generator.Generate());

	for (int vid : mesh.VertexIndicesItr()) {
		FVector3d vertexPosition = mesh.GetVertex(vid);
		vertexPosition.Z = (double)splinePoints[vid].Z;
		mesh.SetVertex(vid, vertexPosition);
	}

	FExtrudeMesh extruder(&mesh);
	extruder.DefaultExtrudeDistance = Height;
	extruder.IsPositiveOffset = Height >= 0;
	extruder.Apply();
}
