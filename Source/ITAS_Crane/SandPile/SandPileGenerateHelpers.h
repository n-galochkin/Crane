// SandPileGenerator.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/DynamicMeshComponent.h"
#include "SandPileGenerateHelpers.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITAS_CRANE_API USandPileGenerateHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType="AccumulatedPoints"))
	static void ProcessPointsAndUpdateMesh(TArray<FVector> AccumulatedPoints, UDynamicMesh* Mesh3);
};
