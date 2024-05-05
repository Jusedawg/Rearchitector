#pragma once
#include "VectorAxisLock.h"

#include "ScaleSettings.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorTargetScale
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, SaveGame)
	double ScaleFactor = 0.1;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FVectorAxisLock AxisLock;
	
	FVector TransformVector(const FVector& ScaleAxis) const { return AxisLock.ApplyLock(ScaleAxis * ScaleFactor); }
};
