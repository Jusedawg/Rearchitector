#pragma once

#include "CoreMinimal.h"
#include "ArchitectorTarget.h"
#include "UObject/Object.h"
#include "ArchitectorTransform.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorMoveTransform
{
	GENERATED_BODY()

public:

	/*
	 * Whether this action is used in this transform data.
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool IsUsed = false;

	/*
	 * Should the object be SET to the specified position instead of MOVED by the specified amount?
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool UseAsSetAction = false;

	UPROPERTY(BlueprintReadWrite, SaveGame) FVector Value = FVector::ZeroVector;

	FArchitectorMoveTransform(){}
	FArchitectorMoveTransform(const FVector& Value, bool UseAsSet = false) : IsUsed(true), UseAsSetAction(UseAsSet), Value(Value){}
	
};

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorRotateTransform
{
	GENERATED_BODY()

public:

	/*
	 * Whether this action is used in this transform data.
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool IsUsed = false;

	/*
	 * Should the object be SET to the specified rotation instead of ROTATED by the specified amount?
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool UseAsSetAction = false;

	UPROPERTY(BlueprintReadWrite, SaveGame) FQuat Value = FQuat::Identity;

	FArchitectorRotateTransform(){}
	FArchitectorRotateTransform(const FQuat& Value, bool UseAsSet = false) : IsUsed(true), UseAsSetAction(UseAsSet), Value(Value){}
};

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorScaleTransform
{
	GENERATED_BODY()

public:

	/*
	 * Whether this action is used in this transform data.
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool IsUsed = false;

	/*
	 * Should the object be SET to the specified scale instead of SCALED by the specified amount?
	 */
	UPROPERTY(BlueprintReadWrite, SaveGame) bool UseAsSetAction = false;

	UPROPERTY(BlueprintReadWrite, SaveGame) FVector Value = FVector::ZeroVector;

	FArchitectorScaleTransform(){}
	FArchitectorScaleTransform(const FVector& Value, bool UseAsSet = false) : IsUsed(true), UseAsSetAction(UseAsSet), Value(Value){}
	
};

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorTransformData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, SaveGame) FArchitectorMoveTransform Move = FArchitectorMoveTransform();
	UPROPERTY(BlueprintReadWrite, SaveGame) FArchitectorRotateTransform Rotate = FArchitectorRotateTransform();
	UPROPERTY(BlueprintReadWrite, SaveGame) FArchitectorScaleTransform Scale = FArchitectorScaleTransform();
};

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorTargetedTransformData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, SaveGame) FArchitectorToolTarget Target = FArchitectorToolTarget();
	UPROPERTY(BlueprintReadWrite, SaveGame) FArchitectorTransformData TransformData = FArchitectorTransformData();
	
	FArchitectorTargetedTransformData(){}
	FArchitectorTargetedTransformData(const FArchitectorToolTarget& Target, const FArchitectorTransformData& TransformData) : Target(Target), TransformData(TransformData){}
};
