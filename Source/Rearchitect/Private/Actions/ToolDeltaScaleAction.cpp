// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaScaleAction.h"

void UToolDeltaScaleAction::PerformAction_Implementation()
{
	FArchitectorTransformData TransformData;
	TransformData.Scale = FArchitectorScaleTransform(Amount);

	GetRCO()->ApplyTransformDataIndependent(Targets, TransformData);
}

void UToolDeltaScaleAction::UndoAction_Implementation()
{
	FArchitectorTransformData TransformData;
	TransformData.Scale = FArchitectorScaleTransform(-Amount);

	GetRCO()->ApplyTransformDataIndependent(Targets, TransformData);
}
