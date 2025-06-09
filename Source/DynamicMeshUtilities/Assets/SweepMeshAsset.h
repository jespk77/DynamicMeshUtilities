#pragma once

#include "CoreMinimal.h"
#include "Algo/Reverse.h"
#include "SweepMeshAsset.generated.h"

#if WITH_EDITOR
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSweepMeshAssetEdited);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSweepMeshMaterialEdited);
#endif

UCLASS(BlueprintType)
class DYNAMICMESHUTILITIES_API USweepMeshAsset : public UObject {
	GENERATED_BODY()

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& event) override {
		Super::PostEditChangeProperty(event);

		if (event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(USweepMeshAsset, MeshMaterial)) OnMaterialEdited.Broadcast();
		else OnAssetEdited.Broadcast();
	}

	virtual void AddOffset(const FVector2D offset) {
		for (FVector2D& point : SweepPoints) point += offset;
		OnAssetEdited.Broadcast();
	}
#endif

public:
	UPROPERTY(Category = "Mesh|Geometry", EditInstanceOnly, BlueprintReadWrite)
	TArray<FVector2D> SweepPoints;

	// A mesh to be placed at certain distances alongside each spline
	UPROPERTY(Category = "Mesh|Geometry", EditInstanceOnly)
	TObjectPtr<UStaticMesh> ExtraMesh;

	// The distance between extra meshes
	UPROPERTY(Category = "Mesh|Geometry", EditInstanceOnly, meta = (ClampMin = 0))
	float ExtraMeshDistance = 250.f;
	// The initial distance from where extra meshes should start spawning
	UPROPERTY(Category = "Mesh|Geometry", EditInstanceOnly, meta = (ClampMin = 0))
	float ExtraMeshStartDistance = 10.f;
	// The end distance from where extra meshes should stop spawning
	UPROPERTY(Category = "Mesh|Geometry", EditInstanceOnly, meta = (ClampMin = 0))
	float ExtraMeshEndDistance = 10.f;

	// Use world units to determine where 1.0 in the UV map is
	UPROPERTY(Category = "Mesh|UV", EditAnywhere, BlueprintReadWrite)
	bool ScaleUVToWorld = true;
	UPROPERTY(Category = "Mesh|UV", EditAnywhere, BlueprintReadWrite, meta = (EditCondition = ScaleUVToWorld))
	float UVWorldUnit = 100.f;

	UPROPERTY(Category = "Mesh|Material", EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MeshMaterial;

	UPROPERTY(Category = "Generation Options", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 5))
	float MaxDistanceFromSpline = 50.f;
	// Whether the ends of the meshes should be capped
	UPROPERTY(Category = "Generation Options", EditAnywhere, BlueprintReadWrite)
	bool EdgeCap = true;

#if WITH_EDITOR
	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual FORCENOINLINE void Flip() {
		Algo::Reverse(SweepPoints);
		OnAssetEdited.Broadcast();
	}

	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual FORCENOINLINE void RoundPoints() {
		for (FVector2D& point : SweepPoints) {
			point.X = FMath::RoundHalfToZero(point.X);
			point.Y = FMath::RoundHalfToZero(point.Y);
		}
		OnAssetEdited.Broadcast();
	}

	// Move all points up a bit
	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual void MoveUp() {
		AddOffset(FVector2D(0, 10));
	}

	// Move all points down a bit
	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual void MoveDown() {
		AddOffset(FVector2D(0, -10));
	}

	// Move all points left a bit
	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual void MoveLeft() {
		AddOffset(FVector2D(-10, 0));
	}

	// Move all points right a bit
	UFUNCTION(Category = "Mesh operations", CallInEditor)
	virtual void MoveRight() {
		AddOffset(FVector2D(10, 0));
	}
#endif

#if WITH_EDITOR
	FOnSweepMeshAssetEdited OnAssetEdited;
	FOnSweepMeshMaterialEdited OnMaterialEdited;
#endif
};
