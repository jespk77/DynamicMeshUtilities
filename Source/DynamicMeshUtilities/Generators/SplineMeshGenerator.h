#pragma once

#include "CoreMinimal.h"
#include "UDynamicMesh.h"
#include "Components/SplineComponent.h"
#include "SplineMeshGenerator.generated.h"

UCLASS(Abstract)
class DYNAMICMESHUTILITIES_API UDynamicMeshSplineGenerator : public UDynamicMeshGenerator {
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Generation Settings", BlueprintReadWrite)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(Category = "Generation Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 5, Delta = 5))
	float MaxDistanceFromSpline = 10.f;
	UPROPERTY(Category = "Generation settings", EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle))
	bool ScaleUVToWorld = true;
	UPROPERTY(Category = "Generation settings", EditAnywhere, BlueprintReadWrite, meta = (EditCondition = ScaleUVToWorld))
	float UVWorldUnit = 1.f;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Generates a mesh following a spline */
UCLASS()
class DYNAMICMESHUTILITIES_API USplinePathGenerator : public UDynamicMeshSplineGenerator {
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Generation Settings", EditAnywhere, BlueprintReadWrite, meta = (Delta = 10))
	float Offset = 0.f;
	UPROPERTY(Category = "Generation Settings", EditAnywhere, BlueprintReadOnly)
	TArray<FVector2D> CrossSection;
	UPROPERTY(Category = "Generation settings", EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ESplineCoordinateSpace::Type> SplineSpace = ESplineCoordinateSpace::Local;
	UPROPERTY(Category = "Generation settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int GroupID = 0;

	virtual void Generate(FDynamicMesh3& mesh) override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Generates a surface mesh using a spline as border */
UCLASS()
class DYNAMICMESHUTILITIES_API USplineSurfaceGenerator : public UDynamicMeshSplineGenerator {
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Generation Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, Delta = 1))
	float Height = 5.f;

	virtual void Generate(FDynamicMesh3& mesh) override;
};
