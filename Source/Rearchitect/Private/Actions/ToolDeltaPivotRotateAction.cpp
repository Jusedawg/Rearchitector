// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaPivotRotateAction.h"

void UToolDeltaPivotRotateAction::PerformAction_Implementation()
{
	TArray<FArchitectorTargetedTransformData> TransformDatas;

	for (const FArchitectorToolTarget& Target : Targets)
	{
		const auto TargetDistance = Target.Target->GetActorLocation() - Origin;
		const auto RotatedTargetDistance = TargetDistance.RotateAngleAxis(Angle, Axis);
		const auto DistanceDelta = RotatedTargetDistance - TargetDistance;

		FArchitectorTargetedTransformData Data;
		Data.Target = Target;
		Data.TransformData.Move = FArchitectorMoveTransform(DistanceDelta);
		Data.TransformData.Rotate = FArchitectorRotateTransform(FQuat(Axis, FMath::DegreesToRadians(Angle)));

		TransformDatas.Add(Data);
	}

	GetRCO()->ApplyIndividualTransformData(TransformDatas);
}

void UToolDeltaPivotRotateAction::UndoAction_Implementation()
{
}
