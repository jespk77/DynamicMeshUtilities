#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DynamicMeshUtilities/Assets/SweepMeshAsset.h"
#include "SweepMeshAssetFactory.generated.h"

UCLASS()
class USweepMeshAssetFactory : public UFactory {
	GENERATED_BODY()

public:
	USweepMeshAssetFactory() {
		SupportedClass = USweepMeshAsset::StaticClass();
		bCreateNew = true;
	}

	virtual UObject* FactoryCreateNew(UClass* objectType, UObject* parent, FName name, EObjectFlags flags, UObject* objectTemplate, FFeedbackContext* context) override {
		return NewObject<USweepMeshAsset>(parent, objectType, name, flags, objectTemplate);
	}
};